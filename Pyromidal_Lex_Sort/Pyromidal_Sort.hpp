#include <stdio.h>
#include "Lexical_Sort.hpp"
void My_Copy_String(char* str1, char* str2, int size_str);
int my_strcmp(const char* str1, const char* str2, size_t len); 
void reffiling(My_String* words_, int size_word_, int size_words_, int i_);
void Po_Sort_Der(My_String* words_, int size_word_, int size_words_);
void Pyromidal_Sort(My_String* words_, int size_word_, int size_words_);