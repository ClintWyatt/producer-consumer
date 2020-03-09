#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

void *thread1();
void *thread2();
void *thread3();
pthread_t t1, t2, t3;//threads 
sem_t sem1, sem2, sem3, mutualExclusion; //semaphores
struct listType
{
	struct listType *next;//head of the linked list free list
	int data;
};


int main()
{
	srand(time(NULL));
	//struct listType *freeList = (struct listType *)malloc(sizeof(struct listType));//allocating memory for the free list
	//struct listType *list1 = (struct listType *)malloc(sizeof(struct listType));//allocating memory for list1
	//struct listType *list2 = (struct listType *)malloc(sizeof(struct listType));//allocating memory for list2
	
	//heads to the linked lists
	struct listType *freeHead = NULL;
	struct listType *list1Head = NULL;
	struct listType *list2Head = NULL;
	struct listType *current = NULL; //used for deleting and adding nodes
	int i; //
	int counter =0; //used for the number of loops required for the program


	//creating the linked list for the free list to 5 nodes
	for(i =0; i < 8; i++)
	{
		if(freeHead == NULL)
		{
			struct listType *node = (struct listType *) malloc(sizeof(struct listType));//creating the first node
			node->next = NULL;
			freeHead = node;
			freeHead->data = rand() % 100;//giving the node a randome number 
		}
		else
		{
			struct listType *node = (struct listType *) malloc(sizeof(struct listType));//creating the first node
			node->next = NULL;
			node->next = freeHead;
			freeHead->data = rand() % 100;
		}
	}


	//creating the threads
	pthread_create(&t1, NULL, (void *) &thread1, NULL);
	pthread_create(&t2, NULL, (void *) &thread2, NULL);
	pthread_create(&t2, NULL, (void *) &thread3, NULL);


	//waiting for threads to finish
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	

	//free memory from free list
	current = freeHead;
	while(current != NULL)
	{
		current = current->next;
		free(freeHead);//deelting alloacted memory
		freeHead = current;
	}

	//free memory from list1
	current = list1Head;
	while(current != NULL)
	{
		current = current->next;
		free(list1Head);
		list1Head = current;
	}
	
	//free memory from list2
	current = list2Head;
	while(current != NULL)
	{
		current = current->next;
		free(list2Head);
		list2Head = current;
	}
	
	return 0;
}

void *thread1()
{




}

void *thread2()
{



}

void *thread3()
{




}

