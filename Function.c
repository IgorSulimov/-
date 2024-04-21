#include "Function.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* Num(char* file)
{
	int* pi = (int*)calloc(strlen(file), sizeof(int));
	pi[0] = 0;
	for (int a = 1; file[a] != '\0'; ++a)
	{
	int l = 0;
	int r = a - 1;
	int pr = r;
	if (file[l] == file[a])
	{
		pi[a] = l + 1;
	}
	while (r != l)
	{
		if (file[l] == file[pr])
		{
			if (l + 1 == (a - r + 1))
			{
				if (pr <= a) {
					pi[a] = l + 1;
					l++;
					pr++;
				}
			}
			else {
				l++;
				pr++;
			}
		}
		else
		{
			l = 0;
			r--;
			pr = r;
		}
		if (pr > a) {
			l = 0;
			r--;
			pr = r;
		}
	}
}
	int str = strlen(file);
for (int d = 0; d < str; d++)
{
	printf("%d", pi[d]);
}
free(pi);
}