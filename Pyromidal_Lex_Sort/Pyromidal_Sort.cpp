#include "Pyromidal_Sort.hpp"

// right_el = i * 2 + 2;
// left_el = i * 2 + 1;
// parent = (i-1) / 2;

void reffiling(My_String* words_, int size_word_, int size_words_, int i_)
{
    int i = i_;
    int k = i;
    int left = i * 2 + 1;
    int right = i * 2 + 2;

    if (left < size_words_ &&
        my_strcmp(words_[left].word, words_[k].word, size_word_) > 0) 
    {
        k = left;
    }

    if (right < size_words_ &&
        my_strcmp(words_[right].word, words_[k].word, size_word_) > 0) {
        k = right;
    }

    if (k != i) {
        char* tmp = new char[size_word_ +1];
        My_Copy_String(tmp, words_[i].word, size_word_);
        My_Copy_String(words_[i].word, words_[k].word, size_word_);
        My_Copy_String(words_[k].word, tmp, size_word_);
        delete[] tmp;

        reffiling(words_, size_word_, size_words_, k);
    }
}

void Po_Sort_Der(My_String* words_, int size_word_, int size_words_)
{
    for (int i = size_words_ - 1; i >= 0; i--)
    {
        reffiling(words_, size_word_, size_words_, i);
    }
}

void Pyromidal_Sort(My_String* words_, int size_word_, int size_words_)
{
    My_String* new_array = new My_String[size_words_];
    for (int i = 0; i < size_words_; i++)
    {
        new_array[i].word = new char[size_word_ + 1];
    }
    int count_element_new_array = 0;
    int j = size_words_;
    Po_Sort_Der(words_, size_word_, size_words_);

    for (int i = size_words_-1; i >= 0; i--)
    {
        char* tmp = new char[size_word_ + 1];
        My_Copy_String(tmp, words_[0].word, size_word_);
        tmp[size_word_] = '\0';
        My_Copy_String(words_[0].word, words_[i].word, size_word_);
        My_Copy_String(words_[i].word, tmp, size_word_);
        delete[] tmp;
        reffiling(words_, size_word_, i, 0);
    }

}
