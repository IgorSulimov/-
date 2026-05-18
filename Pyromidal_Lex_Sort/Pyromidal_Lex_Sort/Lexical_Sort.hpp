#include <stdio.h>

struct My_String
{
    char* word;
};

void My_Copy_String(char* str1, char* str2, int size_str);
int my_strcmp(const char* str1, const char* str2, size_t len);

struct Queue
{
    My_String word_q;   
    Queue* start;           // Указатель на начало очереди
    Queue* end;             // Указатель на конец очереди
    Queue* prev;            // Указатель на предыдущий элемент
    Queue* next;            // Указатель на следующий элемент
    char letter;            // Буква
    int size_word_q;        // Размер очереди


    Queue()
    {
        start = nullptr;
        end = nullptr;
        prev = nullptr;
        next = nullptr;
        word_q.word = nullptr;
        letter = '\0';
        size_word_q = 0;
    }


    ~Queue()
    {
        Queue* current = start;
        while (current != nullptr)
        {
            Queue* next_node = current->next;
            delete current;
            current = next_node;
        }
    }

    void Push_el(My_String element, int size_word)
    {
        Queue* new_node = new Queue();
        new_node->size_word_q = 1;
        new_node->next = nullptr;
        new_node->prev = nullptr;

        new_node->word_q.word = element.word;

        if (start == nullptr && end == nullptr)
        {
            start = new_node;
            end = new_node;
        }
        else
        {
            end->next = new_node;
            new_node->prev = end;
            end = new_node;
        }

        size_word_q++;
    }
    void Push_el_str(My_String element, int size_word)
    {
        Queue* new_node = new Queue();
        new_node->size_word_q = 1;
        new_node->next = nullptr;
        new_node->prev = nullptr;

        new_node->word_q.word = new char[size_word + 1];
        My_Copy_String(new_node->word_q.word, element.word, size_word);

        if (start == nullptr && end == nullptr)
        {
            start = new_node;
            end = new_node;
        }
        else
        {
            end->next = new_node;
            new_node->prev = end;
            end = new_node;
        }

        size_word_q++;
    }
    void Pop_el()
    {
        if (start == nullptr) return;

        start = start->next;
        if (start != nullptr)
        {
            start->prev = nullptr;
        }
        else
        {
            end = nullptr;
        }
        size_word_q--;
    }
};

void Lexical_Sort(My_String* words_, int size_word_, int size_words_);
