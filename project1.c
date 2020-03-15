#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

void *thread1();
void *thread2();
void *thread3();
pthread_t t1, t2, t3;//threads 
sem_t freeList, list1, list2, mx; //semaphores
struct listType
{
	struct listType *next;//head of the linked list free list
	int data;
};
int counter =0; //used for the number of loops required for the program
int limit = 250;
struct listType *freeHead = NULL;
struct listType *list1Head = NULL;
struct listType *list2Head = NULL;
struct listType *current = NULL; //used for deleting and adding nodes


int main()
{
	srand(time(NULL));
	//struct listType *freeList = (struct listType *)malloc(sizeof(struct listType));//allocating memory for the free list
	//struct listType *list1 = (struct listType *)malloc(sizeof(struct listType));//allocating memory for list1
	//struct listType *list2 = (struct listType *)malloc(sizeof(struct listType));//allocating memory for list2


	int i; 

	//creating the linked list for the free list to 8 nodes
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

	sem_init(&freeList, 0, 7);//initializing the counting semaphore for the free list. 1 less than the number of nodes in the free list
	//If the count of the semaphore is not 1 less, then the program will segfault in thread2. 
	sem_init(&list1, 0, 0);//initializing the counting semaphore for list1
	sem_init(&list2, 0, 0); //initializing the counting semaphore for list2
	sem_init(&mx, 0, 1);//initializing the binary semaphore for mutual exclusion

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
	struct listType *trailPtr = NULL; //used to have the front of the list go to list1
	int val; //used to get the semaphore value
	while(counter < limit)
	{
		sem_wait(&freeList);
		sem_wait(&mx);//locking other threads out of the critical section
		//add to list 1 and take away from free list
		trailPtr = freeHead;
		freeHead = freeHead->next; //resetting the head of the freelist to the next node
		if(list1Head == NULL)
		{
			list1Head = trailPtr;
			trailPtr = NULL;
		}
		else
		{
			trailPtr->next = list1Head;//resetting the node from the freelist to list1's head
			list1Head = trailPtr;//resetting the head of list1
			trailPtr = NULL;//having trailPtr point to NULL
		}
		counter++;
		sem_post(&list1);//element has been added to list1. 
		sem_post(&mx);//put any of the threads that are blocked on the ready queue

	}

}

void *thread2()
{
	struct listType *list1Ptr = NULL;
	struct listType *freelistPtr = NULL;

	while(counter < limit)
	{
		sem_wait(&list1);
		sem_wait(&mx);
		//getting the node from list 1 
		list1Ptr = list1Head;
		list1Head = list1Ptr->next;

		//getting the node from the free list
		freelistPtr = freeHead;
		freeHead = freelistPtr->next;

		//adding node from list1 to the freelist
		if(list1Head == NULL) //check if inital list is empty
		{
			list1Head = list1Ptr;
			list1Ptr = NULL;
		}
		else
		{
			list1Ptr->next = freeHead; //reset freelist head to new node from list 1
			freeHead = list1Ptr; //Set the new freelist head
			list1Ptr = NULL; //set un-needed pointer to NULL
		}
		

		//adding node from the freelist to list2
		if(list2Head == NULL) //check if inital list is empty
		{
			list2Head = freelistPtr;
			freelistPtr = NULL;
		}
		else
		{
			freelistPtr->next = list2Head; //reset list 2 head to new node from freelist 
			list2Head = freelistPtr; //Set the new list 2 head
			freelistPtr = NULL; //set un-needed pointer to NULL
		}
		

		counter++;
		sem_post(&list2);//node has been added to list2
		sem_post(&mx);//allow another thread to access the critical section
		
	}

}

void *thread3()
{
	while(counter < limit)
	{
		sem_wait(&list2);
		sem_wait(&mx);

		//take node from list2 and add it to the free list
		
		
		counter++;
		sem_post(&freeList); //node has been added to the free list
		sem_post(&mx); //allow another thread to access the critical section


	}

}

