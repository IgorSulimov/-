#include "Lexical_Sort.hpp"
void My_Copy_String(char* str1, char* str2, int size_str)
{
    for (int i = 0; i < size_str; i++)
    {
        str1[i] = str2[i];
    }
    str1[size_str] = '\0';
}

int my_strcmp(const char* str1, const char* str2, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (str1[i] != str2[i]) {
            return (unsigned char)str1[i] - (unsigned char)str2[i]; // положительное - str1 > str2, отрицательное - str2 > str1
        }
    }
    return 0;//одинаковые
}
void Lexical_Sort(My_String* words_, int size_word_, int size_words_)
{
    Queue main_que;
    Queue queues[26];

    for (int i = 0; i < 26; i++)
    {
        queues[i].letter = 'a' + i;
        queues[i].start = nullptr;
        queues[i].end = nullptr;
        queues[i].size_word_q = 0;
    }

    for (int words = 0; words < size_words_; words++)
    {
        main_que.Push_el(words_[words], size_word_);
    }


    for (int k = size_word_ - 1; k >= 0; k--)
    {
        int current_size = main_que.size_word_q;

        for (int i = 0; i < current_size; i++)
        {
            char letter_ = main_que.start->word_q.word[k];
            int queue_index = letter_ - 'a';

            if (queue_index >= 0 && queue_index < 26)
            {
                // Берём указатель на текущий узел
                Queue* current_node = main_que.start;

                // Удаляем из main_que
                main_que.Pop_el();

                // Очищаем связи узла
                current_node->next = nullptr;
                current_node->prev = nullptr;

                // Добавляем в побочную очередь
                if (queues[queue_index].start == nullptr)
                {
                    queues[queue_index].start = current_node;
                    queues[queue_index].end = current_node;
                }
                else
                {
                    queues[queue_index].end->next = current_node;
                    current_node->prev = queues[queue_index].end;
                    queues[queue_index].end = current_node;
                }
                queues[queue_index].size_word_q++;
            }
        }

        // Соединяем побочные очереди
        Queue* first_start = nullptr;
        Queue* last_end = nullptr;

        for (int t = 0; t < 26; t++)
        {
            if (queues[t].start != nullptr)
            {
                if (first_start == nullptr)
                {
                    first_start = queues[t].start;
                    last_end = queues[t].end;
                }
                else
                {
                    last_end->next = queues[t].start;
                    queues[t].start->prev = last_end;
                    last_end = queues[t].end;
                }
            }
        }

        // Восстанавливаем main_que
        if (first_start != nullptr)
        {
            main_que.start = first_start;
            main_que.end = last_end;
            main_que.size_word_q = current_size;
        }

        // Очищаем побочные очереди
        for (int t = 0; t < 26; t++)
        {
            queues[t].start = nullptr;
            queues[t].end = nullptr;
            queues[t].size_word_q = 0;
        }
    }

    Queue* current = main_que.start;
    int index = 0;
    while (current != nullptr && index < size_words_)
    {
        char* temp = words_[index].word;
        words_[index].word = current->word_q.word;
        current->word_q.word = temp;

        current = current->next;
        index++;
    }
}

