#ifndef PARSER_H_
#define PARSER_H_

#define MAX_COMMANDS 16
#define MAX_ARGV     16

/**
 *  Позиция команды в конвейере commd.
 */
typedef enum {single, first, middle, last, unknown} position_t;

/**
 * Структура, содержащая все данные для одной команды в конвейере команд.
 */
typedef struct {
  char*  argv[MAX_ARGV];  // Вектор аргументов.
  position_t pos;         // Позиция в конвейере (одиночная, первая, средняя или последняя).
  int in;                 // Дескриптор входного файла.
  int out;                // Дескриптор выходного файла.
} cmd_t;

/**
 * Разбирает строку str и заполняет массив commands данными для каждой
 * команды.
 */
int parse_commands(char *str, cmd_t* commands);

/**
 * Выводит элементы вектора аргументов argv.
 */
void print_argv(char* argv[]);

/**
 * Преобразует позицию pos_t (одиночная, первая, средняя или последняя) в строку
 * ("single", "first", "middle" или "last").
 */
char* position_to_string(position_t pos);

#endif // PARSER_H_
