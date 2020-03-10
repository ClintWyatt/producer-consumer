#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

void *thread1();
void *thread2();
void *thread3();
pthread_t t1, t2, t3;//threads 
sem_t freeList, list1, list2, mx, freeBlock; //semaphores
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
	int i; //shared variable for looping
	int counter =0; //used for the number of loops required for the program
	int limit = 250;

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
	
	sem_init(&freeList, 0, 8);//initializing the counting semaphore for the free list
       	sem_init(&list1, 0, 0);//initializing the counting semaphore for list1
	sem_init(&list2, 0, 0)l //initializing the counting semaphore for list2
	sem_init(&mx, 0, 1);//initializing the binary semaphore for mutual exclusion
	sem_init(&freeBlock, 0, 0); //initializing the binary semaphore freeblcok, which is used when the free list has 1 node
		
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
	while(counter < limit)
	{
		sem_wait(&freeList);
		sem_wait(&mx);
		//add to list 1 and take away from free list
		

		sem_post(&list1);//element has been added to list1. 
		sem_post(&mx);//put any of the threads that are blocked on the ready queue
		counter ++;
	}

}

void *thread2()
{
	while(counter < limit)
	{
		sem_wait(&list1);
		sem_wait(&mx);
		//take 

	}

}

void *thread3()
{
	while(counter < limit)
	{



	}

}

