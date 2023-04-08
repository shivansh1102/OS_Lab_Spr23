#include <stdio.h>
#include <signal.h>

void handler(int signum)
{
	printf("\nI am unstoppable!\n");
}

int main()
{	
	signal(SIGINT, handler);
	char curr;
	while(1)
	{
		scanf("%c", &curr);
		getchar();
		if(curr == 'x')
		{
			printf("Valar Morghulis\n");
			break;
		}
		if(!((curr >= 'a' && curr <= 'z') || (curr >= 'A' && curr <= 'Z')))
		{
			printf("Do you speak-a my language?\n");
			continue;
		}
		printf("%c\n", curr);
	}
	return 0;
}
