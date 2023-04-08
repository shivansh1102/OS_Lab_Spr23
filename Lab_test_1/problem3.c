#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


#define UNPROCESSED 0
#define PROCESSED 1
int n, m;
int indexAB, cntProcessed;
pthread_mutex_t lockAB, lockRow[1000], lockCnt;

int **M, *A, *B;
void sort(int *arr)
{
	for(int i = 0; i < n; i++)
	{
		for(int j = i+1; j < n; j++)
		{
			if(arr[j] < arr[i])
			{
				int temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
		}
	}
}

void* chaosThread(void *param)
{
	printf("I am chaos\n");
	
	int row, col, newval;
	indexAB = 0;
	for(int i = 0; i < 30; i++)
	{
		row = rand()%m;
		col = rand()%n;
		newval = rand()%1000 + 1;
		
		pthread_mutex_lock(&lockRow[row]);
		M[row][col] = newval;
		pthread_mutex_unlock(&lockRow[row]);
		
		printf("Chaos : updated element at cell %d*%d with value %d\n", row, col, newval);
		
		pthread_mutex_lock(&lockAB);
		A[indexAB] = row;
		B[indexAB] = UNPROCESSED;
		pthread_mutex_unlock(&lockAB);
		indexAB++;
		
		sleep(2);
	}
	printf("Chaos ends\n");
	pthread_exit(0);
}

void* orderThread(void *param)
{
	printf("I am order\n");
	
	int curr, currrow;
	while(1)
	{
		pthread_mutex_lock(&lockCnt);
		if(cntProcessed == 30)
		{
			pthread_mutex_unlock(&lockCnt);
			break;
		}
		pthread_mutex_unlock(&lockCnt);
		
		curr = -1;
		pthread_mutex_lock(&lockAB);
		for(int i = 0; i < 30; i++)
		{
			if(B[i] == UNPROCESSED)
			{
				curr = i;
				break;
			}
		}
		if(curr == -1)
		{
			pthread_mutex_unlock(&lockAB);
			continue;
		}
		
		currrow = A[curr];
		B[curr] = PROCESSED;
		pthread_mutex_unlock(&lockAB);
		
		printf("Order Detected updated element at row %d\n", currrow);
		
		printf("Order: row %d is sorted now\n", currrow);
		
		pthread_mutex_lock(&lockRow[currrow]);
		printf("Older row %d: ", currrow);
		for(int i = 0; i < n; i++)
		printf("%d ", M[currrow][i]);
		printf("\n");
		sort(M[currrow]);
		printf("New row %d: ", currrow);
		for(int i = 0; i < n; i++)
		printf("%d ", M[currrow][i]);
		printf("\n");
		pthread_mutex_unlock(&lockRow[currrow]);
		
		pthread_mutex_lock(&lockCnt);
		cntProcessed++;
		pthread_mutex_unlock(&lockCnt);
	}
}

int main()
{
	srand(time(NULL));
	indexAB = 0; cntProcessed = 0;
	scanf("%d %d", &n, &m);
	M = (int**)malloc(m*sizeof(int*));
	for(int i = 0; i < m; i++)
	{
		M[i] = (int*)malloc(n*sizeof(int));
		for(int j = 0; j < n; j++)
		M[i][j] = rand()%1000 + 1;
	}
	printf("Random matrix M is created\n");
	
	A = (int*) malloc(1000*sizeof(int));
	B = (int*) malloc(1000*sizeof(int));
	for(int i = 0; i < 1000; i++)
	B[i] = -1;
	printf("Shared arrays A and B are created\n");	
	
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < n; j++)
		printf("%d ", M[i][j]);
		printf("\n");
	}
	
	pthread_mutex_init(&lockAB, NULL);
	pthread_mutex_init(&lockCnt, NULL);
	for(int i = 0; i < 1000; i++)
	pthread_mutex_init(&lockRow[i], NULL);
	
	pthread_t ord1, ord2, ord3, chao;
	pthread_attr_t attrord1, attrord2, attrord3, attrchao;
	pthread_attr_init(&attrord1);
	pthread_attr_init(&attrord2);
	pthread_attr_init(&attrord3);
	pthread_attr_init(&attrchao);
		
	pthread_create(&chao, &attrchao, chaosThread, NULL);
	pthread_create(&ord1, &attrord1, orderThread, NULL);
	pthread_create(&ord2, &attrord2, orderThread, NULL);
	pthread_create(&ord3, &attrord3, orderThread, NULL);
	
	pthread_join(ord1, NULL);
	pthread_join(ord2, NULL);
	pthread_join(ord3, NULL);
	pthread_join(chao, NULL);
	
	pthread_mutex_destroy(&lockAB);
	pthread_mutex_destroy(&lockCnt);
	for(int i = 0; i < 1000; i++)
	pthread_mutex_destroy(&lockRow[i]);
	
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < n; j++)
		printf("%d ", M[i][j]);
		printf("\n");
	}
	
	return 0;
}