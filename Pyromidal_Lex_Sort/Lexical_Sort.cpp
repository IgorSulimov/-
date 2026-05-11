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
    int index_queues = 0;

    for (int i = 0; i < 26; i++)
    {
        queues[i].letter = 'a' + i;
    }

    for (int words = 0; words < size_words_; words++)
    {
        main_que.Push_el(words_[words], size_word_);
    }
    // k - Ќомер буквы, которую мы берЄм в слове
    for (int k = size_word_ - 1; k >= 0; k--)
    {
        int current_size = main_que.size_word_q;

        for (int i = 0; i < current_size; i++)
        {
            char letter_ = main_que.start->word_q.word[k];

            int queue_index = letter_ - 'a';

            if (queue_index >= 0 && queue_index < 26)
            {
                queues[queue_index].Push_el(main_que.start->word_q, size_word_);
            }

            main_que.Pop_el();
        }

        // —обираем все слова обратно в главную очередь
        for (int t = 0; t < 26; t++)
        {
            while (queues[t].start != nullptr)
            {
                main_que.Push_el(queues[t].start->word_q, size_word_);
                queues[t].Pop_el();
            }
        }
    }

    //  опируем отсортированные слова обратно в массив
    Queue* current = main_que.start;
    int index = 0;
    while (current != nullptr && index < size_words_)
    {
        My_Copy_String(words_[index].word, current->word_q.word, size_word_);
        current = current->next;
        index++;
    }
}

