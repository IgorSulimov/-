#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;
#include "Pyromidal_Sort.hpp"
#include <thread>
int size_words = 0;
int size_word = 0;

void create_rand_word(char* word_, int size_word_)
{
    for (int i = 0; i < size_word_; i++)
    {
        word_[i] = 97 + rand() % 26;
    }
    word_[size_word_] = '\0';
}

void read_words_from_file(const char* filename, My_String*& words)
{
    FILE* file;
    errno_t err = fopen_s(&file, filename, "r");

    if (err != 0 || file == NULL)
    {
        printf("Не удалось открыть файл: %s\n", filename);
        size_words = 0;
        size_word = 0;
        return;
    }

    char buffer[256];
    int total_words = 0;
    int word_length = -1;

    while (fgets(buffer, 256, file))
    {
        int current_len = 0;
        while (buffer[current_len] != '\n' && buffer[current_len] != '\0' && current_len < 256)
        {
            current_len++;
        }

        if (word_length == -1)
        {
            word_length = current_len;
        }
        else if (current_len != word_length)
        {
            printf("Ошибка: В файле слова разной длины!\n");
            fclose(file);
            size_words = 0;
            size_word = 0;
            return;
        }
        total_words++;
    }

    size_words = total_words;
    size_word = word_length;

    if (size_words == 0 || size_word <= 0)
    {
        printf("Файл пуст или слова имеют нулевую длину!\n");
        fclose(file);
        return;
    }

   
    words = new My_String[size_words];

    for (int i = 0; i < size_words; i++)
    {
        words[i].word = new char[size_word + 1];
    }

    rewind(file);

    int current_index = 0;
    while (fgets(buffer, 256, file) && current_index < size_words)
    {
        int len = 0;
        while (buffer[len] != '\n' && buffer[len] != '\0' && len < size_word)
        {
            words[current_index].word[len] = buffer[len];
            len++;
        }

        for (int i = len; i < size_word; i++)
        {
            words[current_index].word[i] = ' ';
        }
        words[current_index].word[size_word] = '\0';
        current_index++;
    }

    fclose(file);
}

int main()
{
    srand(time(NULL));
    My_String* words1 = nullptr;
    My_String* words2 = nullptr;
    setlocale(LC_ALL, "");
    int variants;
    cout << "Выберете способ заполнения слов:" << endl;
    cout << "1.Файл" << endl;
    cout << "2.Рандомные слова" << endl;
    cin >> variants;
    switch (variants)
    {
    case 1:
    {
        char file[120];
        cout << "Введите имя файла" << endl;
        cin >> file;
        read_words_from_file(file, words1);
        words2 = new My_String[size_words];
        for (int i = 0; i < size_words; i++)
        {
            words2[i].word = new char[size_word + 1];
            My_Copy_String(words2[i].word, words1[i].word, size_word);
        }
        break;
    }
    case 2:
    {
        cout << "Введите количество слов: ";
        cin >> size_words;
        cout << "Введите размер слов (один для всех): ";
        cin >> size_word;

        cout << "Размер слов: " << size_word << endl;
        cout << "Количество слов: " << size_words << endl << endl;

        words1 = new My_String[size_words];
        words2 = new My_String[size_words];
        for (int i = 0; i < size_words; i++)
        {
            words1[i].word = new char[size_word + 1];
            create_rand_word(words1[i].word, size_word);
        }
        for (int i = 0; i < size_words; i++)
        {
            words2[i].word = new char[size_word + 1];
            My_Copy_String(words2[i].word, words1[i].word, size_word);
        }
        break;
    }
    default:
        cout << "Неверный выбор!" << endl;
        return -1;

    }

    cout << "Размер слов: " << size_word << endl;
    cout << "Количество слов: " << size_words << endl << endl;


    //cout << "\nBefore sorting:\n";
    //for (int i = 0; i < size_words; i++)
    //{
    //    cout << words1[i].word << endl;
    //}

    clock_t start1, start2;
    clock_t end1, end2;
    double time1, time2;

    start1 = clock();
    thread th1(Pyromidal_Sort, words1, size_word, size_words);
    start2 = clock();
    thread th2(Lexical_Sort, words2, size_word, size_words);
    th1.join();
    end1 = clock();
    th2.join();
    end2 = clock();
    time1 = ((double)(end1 - start1)) / CLOCKS_PER_SEC;
    time2 = ((double)(end2 - start2)) / CLOCKS_PER_SEC;

    cout << "Pyromidal_Sort:" << time1  << endl;
    cout << "Lexical_Sort:" << time2  << endl;
    cout << "\nPyromidal_Sort:\n";
    for (int i = 0; i < size_words; i++)
    {
        cout << words1[i].word << endl;
    }
        cout << "\nLexical_Sort:\n";
    for (int i = 0; i < size_words; i++)
    {
        cout << words2[i].word << endl;
    }
    for (int i = 0; i < size_words; i++)
    {
        delete[] words1[i].word;
        delete[] words2[i].word;
    }
    delete[] words1;
    delete[] words2;
    return 0;
}
