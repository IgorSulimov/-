#include <stdio.h>
#include <stdlib.h>

int Strlen(char* str)
{
int count = 0;
for(int d =0; str[d] != '\0';d++)
{
count = count + 1;
}
return count;
}

void* Num(char* file)
{
int* pi = (int*)calloc(Strlen(file), sizeof(int));
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
		if (pr > a) 
		{
		l = 0;
		r--;
		pr = r;
		}
	}
	}
	int str = Strlen(file);
	for (int d = 0; d < str; d++)
	{
	printf("%d", pi[d]);
	}
	free(pi);
	return 0;
}

int main()
{
	FILE* file;
	file = fopen( "TOI.txt", "w");
	if (file == NULL)
		return -1;
	fprintf(file, "%s", "abacabadava");
	fclose(file);

	FILE* in;
	char buff[100];
	in = fopen("TOI.txt", "r");
	if (in == NULL)
		return -2;
	while (!feof(file)) 
	{
		fscanf(file, "%s\n", buff);
		printf("\n%s\n", buff);
		Num(buff);
	}
	fclose(in);
	return 0;
}



