#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

// =========================================================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ДЛЯ СВЯЗИ ПОТОКОВ (ОЧЕРЕДЬ ЗАДАЧ)
// =========================================================================
std::queue<SOCKET> taskQueue;           // Очередь, куда главный поток кладет сокеты клиентов
std::mutex queueMutex;                  // Мьютекс для защиты очереди от одновременного доступа
std::condition_variable queueCV;        // Условная переменная: усыпляет/будит рабочие потоки
bool serverRunning = true;              // Флаг для graceful shutdown (плавной остановки)

// =========================================================================
// ФУНКЦИЯ РАБОЧЕГО ПОТОКА (THREAD POOL)
// =========================================================================
// Эти потоки ничего не знают про accept и select. Они просто спят, 
// пока главный поток не кинет им сокет в очередь.
void WorkerThread() {
    while (serverRunning) {
        SOCKET clientSocket = INVALID_SOCKET;

        // 1. Захватываем мьютекс и ждем, пока появится задача
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            
            // Поток "засыпает" здесь, пока не выполнятся условия:
            // - в очереди есть задача (!taskQueue.empty())
            // - или сервер выключается (!serverRunning)
            queueCV.wait(lock, [] { return !taskQueue.empty() || !serverRunning; });
            
            // Если сервер выключается и задач больше нет — выходим из потока
            if (!serverRunning && taskQueue.empty()) break;
            
            // 2. Берем сокет из очереди
            clientSocket = taskQueue.front();
            taskQueue.pop();
        } // Мьютекс отпускается здесь, чтобы другие потоки тоже могли брать задачи

        if (clientSocket == INVALID_SOCKET) continue;

        // --- 3. ОБРАБОТКА ЗАПРОСА КЛИЕНТА ---
        char recvbuf[DEFAULT_BUFLEN];
        int iResult;

        printf("[Worker %lu] Started handling socket %llu\n", GetCurrentThreadId(), (unsigned long long)clientSocket);

        // Читаем данные от клиента (блокирующий recv, так как это уже отдельный поток)
        iResult = recv(clientSocket, recvbuf, DEFAULT_BUFLEN - 1, 0);
        
        if (iResult > 0) {
            recvbuf[iResult] = '\0'; // Добавляем нуль-терминатор для безопасного вывода
            printf("[Worker] Received %d bytes: %s\n", iResult, recvbuf);

            // Отправляем данные обратно (эхо)
            int iSendResult = send(clientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("[Worker] send failed with error: %d\n", WSAGetLastError());
            } else {
                printf("[Worker] Echoed %d bytes back.\n", iSendResult);
            }
        } 
        else if (iResult == 0) {
            printf("[Worker] Client disconnected.\n");
        } 
        else {
            printf("[Worker] recv failed with error: %d\n", WSAGetLastError());
        }

        // Обязательно закрываем сокет, когда закончили с ним работу
        closesocket(clientSocket);
        printf("[Worker %lu] Finished and closed socket.\n\n", GetCurrentThreadId());
    }
}

// =========================================================================
// ГЛАВНАЯ ФУНКЦИЯ (EVENT LOOP)
// =========================================================================
int main() {
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL;
    struct addrinfo hints;
    int iResult;

    // 1. Инициализация Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // 2. Создание сокета
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // 3. Биндинг и прослушивание
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // 4. ПЕРЕВОДИМ СЛУШАЮЩИЙ СОКЕТ В НЕБЛОКИРУЮЩИЙ РЕЖИМ (Требование преподавателя)
    u_long mode = 1;
    if (ioctlsocket(ListenSocket, FIONBIO, &mode) != 0) {
        printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // 5. ЗАПУСК ПУЛА РАБОЧИХ ПОТОКОВ (Например, 4 потока)
    const int NUM_WORKERS = 4;
    std::vector<std::thread> workers;
    for (int i = 0; i < NUM_WORKERS; ++i) {
        workers.emplace_back(WorkerThread);
    }

    printf("Server started on port %s with %d worker threads.\n", DEFAULT_PORT, NUM_WORKERS);
    printf("Main thread (Event Loop) is now watching for new connections...\n\n");

    // 6. ГЛАВНЫЙ ЦИКЛ ОБРАБОТКИ СОБЫТИЙ (EVENT LOOP)
    // Здесь мы не обрабатываем данные, мы только ловим новые подключения!
    while (serverRunning) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(ListenSocket, &readfds);

        // select блокируется, но не намертво как accept(). 
        // Он просыпается, как только ListenSocket готов принять нового клиента.
        int activity = select(0, &readfds, NULL, NULL, NULL);

        if (activity == SOCKET_ERROR) {
            printf("select failed with error: %d\n", WSAGetLastError());
            break;
        }

        // Если активность есть, значит кто-то подключается
        if (FD_ISSET(ListenSocket, &readfds)) {
            SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
            
            if (ClientSocket != INVALID_SOCKET) {
                printf("[Main] New client connected (Socket: %llu). Sending to queue...\n", (unsigned long long)ClientSocket);
                
                // Кладем сокет в очередь
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    taskQueue.push(ClientSocket);
                }
                // Будим один из спящих потоков, чтобы он забрал задачу
                queueCV.notify_one();
            }
        }
    }

    // =========================================================================
    // ЗАВЕРШЕНИЕ РАБОТЫ (Graceful Shutdown)
    // =========================================================================
    printf("Shutting down server...\n");
    
    serverRunning = false;      // Даем команду потокам завершаться
    queueCV.notify_all();       // Будим все потоки, чтобы они увидели флаг serverRunning

    // Ждем, пока все потоки корректно завершат свою работу
    for (auto& w : workers) {
        if (w.joinable()) w.join();
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}