#include "parser.h"

#include <unistd.h>  // STDIN_FILENO, STDOUT_FILENO
#include <stdlib.h>  // EXIT_FAILURE
#include <stdio.h>   // printf()
#include <string.h>  // strlen()
#include <ctype.h>   // isspace(),
#include <string.h>  // strtok_r()
#include <stdbool.h> // true, false

/**
 * Возвращает true, если строка содержит только пробелы. В противном случае
 * возвращается false.
 */
bool empty(char* str) {
  while (*str) {
    if (!isspace(*str++))
      return false;
  }
  return true;
}

/**
 * Удаляет пробельные символы (табуляции, пробелы и т.д.) из начала 
 * строки.
 */
char* ltrim(char* s) {
  while (isspace((int)*s)) ++s;
  return s;
}

/**
 * Удаляет пробельные символы (табуляции, пробелы и т.д.) из конца 
 * строки.
 */
char* rtrim(char* str) {
  if(*str == 0 )
    return str;
  else {
    char* back = str + strlen(str)-1;  // Указатель на последний символ в строке.

    while( isspace( *back) ) back--;   // Если в конце пробела перемещается указатель-

    *(back+1) = 0; // Добавляем завершающий '\0'.

    return str;

  }
}

/**
 * Удалите пробельные символы (табуляции, пробелы и т.д.) из начала и
 * конца строки.
 */
char* trim(char* str) {
  if (str != NULL)
    return(ltrim(rtrim(str)));
  else return NULL;
}

/**
 * Заменяет символ разделителя в str на NULL и заполняет массив argv
 * указателями на каждый токен. Каждый такой указатель указывает на 
 * вложенную строку в str.
 */
void get_argv(char* str, const char* delim, char* argv[]) {

  char *token;
  int i = 0;

  /* получаем первый токен */
  token = strtok(str, delim);

  /* идем по другим токенам */
  while( token != NULL ) {
    argv[i] = token;
    token = strtok(NULL, delim);
    i++;
  }
  argv[i] = NULL;
}

/**
 * Печатаем элементы вектора argv.
 */
void print_argv(char* argv[]) {
  int i = 0;
  char* s;

  while ((s = argv[i])) {
    printf("  argv[%d] = %s\n", i, s);
    i++;
  }
}

/**
 * Разделяет строку str при каждой встрече с символом разделителя, заменяя
 * каждый символ разделителя на NULL. Наполняет массив cmds указателями на
 * каждую подкоманду. Каждый такой указатель является указателем на подстроку в str.
 */
void parse_cmds(char* str, const char* delim, char* cmds[]) {

  char *token;
  int i = 0;

  /* получить первый токен */
  token = strtok(str, delim);

  /* идем по другим токенам\ */
  while( token != NULL ) {
    if (empty(token)) {
      fprintf(stderr, "Parser error: EMPTY command!\n");
      exit(EXIT_FAILURE);
    }

    cmds[i] = trim(token);
    token = strtok(NULL, delim);
    i++;
  }
  cmds[i] = NULL;
}

/**
 * Преобразует позицию pos_t (одиночная, первая, средняя или последняя) в строку
 * ("single", "first", "middle" или "last").
 */
char* position_to_string(position_t pos) {

  switch (pos) {
  case single:
    return "single";
  case first:
    return "first";
  case middle:
    return "middle";
  case last:
    return "last";
  case unknown:
    return "unknown";
  default:
    return "?????";
  }
}

/**
 * n - количество команд.
 * i - индекс команды (первая команда имеет индекс 0).
 *
 * Возвращает позицию (одиночную, первую, среднюю или последнюю) 
 * команды по индексу i.
 */
position_t cmd_position(int i, int n) {
  if (i == 0 && n == 1) return single;
  if (i == 0 && n > 1) return first;
  if (i == n - 1 && n > 1) return last;
  if (i > 0 && i < n - 1) return middle;
  return unknown;
}

/**
 *  Заполняет массив commands векторами аргументов для всех команд в строке str.
 *
 *  Инициализация каждой команды:
 *     - position             в unknown
 *     - in дескриптор файла  в STDIN
 *     - out дескриптор файла в STDOUT
 *
 *  Возвращает количество разобранных команд.
 */
int first_pass(char *str, cmd_t* commands) {
  char* cmds[MAX_COMMANDS];

  parse_cmds(str, "|", cmds);

  int i = 0;

  while (cmds[i]) {
    commands[i].pos = unknown;
    get_argv(cmds[i], " ", commands[i].argv);
    commands[i].in   = STDIN_FILENO;
    commands[i].out  = STDOUT_FILENO;
    i++;
  }

  return i;
}

/**
 * После первого прохода мы можем узнать позицию каждой команды (одиночная, первая,
 * средняя или последняя).
 */
void second_pass(cmd_t* commands, int n){
  for (int i = 0; i < n; i++) {
    commands[i].pos = cmd_position(i, n);
  }
}

/**
 * Разбирает строку str и заполняет массив commands данными для каждой
 * команды.
 */
int parse_commands(char *str, cmd_t* commands) {
  int n = first_pass(str, commands);
  second_pass(commands, n);
  return n;
}
