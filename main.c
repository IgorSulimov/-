#include <stdio.h>
#include "Function.h"
#include "string.h"
#include <stdlib.h>
#include <time.h>
int main()
{
	char* line[100];
	FILE* file;
	fopen_s(&file, "E:\TOI.txt", "r");
	if (file == NULL)
		return -1;
	fprintf(file, "%s", "da dady pop\njopa\nda da da ti krasivaya\n");
	fclose(file);

	FILE* in;
	char l;
	int i = 0;
	char buff[100];
	fopen_s(&in, "E:\TOI.txt", "r");
	if (in == NULL)
		return -2;
	while (!feof(file)) 
	{
		fscanf_s(file, "%s\n", &buff, (unsigned)_countof(buff));
		printf("%s\n", buff);
		printf("%d\n\n",Num(buff));
	}
	fclose(in);
	return 0;
}