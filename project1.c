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
	for( i =0; i < 8; i++)
        {
                if(freeHead == NULL)
                {
                        struct listType *node = (struct listType *)malloc(sizeof(struct listType));
                        node->next = NULL;//doing this to prevent uninialtized pointers
			node->data = 0;
                        freeHead = node;
                }
                else
                {
                        struct listType *node = (struct listType *)malloc(sizeof(struct listType));
                        node->next = NULL;
                        node->next = freeHead;
                        freeHead = node;
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
	struct listType *current;
        int val; //used to get the semaphore value
        struct listType *trailPtr = NULL; //used to have the front of the list go to list1
        while(counter < limit)
        {
                sem_wait(&freeList);
                sem_wait(&mx);//locking other threads out of the critical section
                //add to list 1 and take away from free list

		printf("Thread 1:\n");

                current = freeHead;
                printf("Nodes in the free list: ");
                while(current != NULL)
                {
			if(current->data ==0)
				current->data = rand() %100 + 1;//randomizing the data for the node


                        printf("%d -> ", current->data);//printing the data in the free list
                        current = current->next;//advancing to the next node
                }
                printf("\n");

                if(list1Head == NULL)
                {
                        list1Head = freeHead;//setting the head of list1 to the head of the free list
                        freeHead = freeHead->next;//advancing the free list to the next node
                        list1Head->next = NULL;//having the next of list1's head be null
                }
                else
                {
                        trailPtr = freeHead;//setting the trailPtr to the free list's head
                        freeHead = freeHead->next;//advancing the free list to the next node
                        trailPtr->next = list1Head;//having the original freelist head point to the head of list1
                        list1Head = trailPtr;//resetting the head for list1
                }
                trailPtr = NULL;
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

		printf("Thread 2:\n");

		list1Ptr = list1Head;
		printf("Nodes in list 1: ");
		while(list1Ptr != NULL)
		{
			printf("%d -> ", list1Ptr->data);
			list1Ptr = list1Ptr->next;
		}
		printf("\n");

		//unlink node from list 1 
		list1Ptr = list1Head;
		list1Head = list1Ptr->next;
		list1Ptr->next = NULL;

		//unlink node from the free list
		freelistPtr = freeHead;
		freeHead = freelistPtr->next;
		freelistPtr->next = NULL;

		//Use list1Ptr(x) to produce freelistPtr(y) data
		printf("Data before transfer:\n");
		printf("Node X: %d\tNode Y: %d\n", list1Ptr->data, freelistPtr->data);
		freelistPtr->data = list1Ptr->data;


		//link node from list1 to the freelist
		if(freeHead == NULL) //check if inital list is empty
		{
			freeHead = list1Ptr;
		}
		else
		{
			list1Ptr->next = freeHead; //reset freelist head to new node from list 1
			freeHead = list1Ptr; //Set the new freelist head
		}
		

		//link node from the freelist to list2
		if(list2Head == NULL) //check if inital list is empty
		{
			list2Head = freelistPtr;
		}
		else
		{
			freelistPtr->next = list2Head; //reset list 2 head to new node from freelist 
			list2Head = freelistPtr; //Set the new list 2 head
		}
		
		//set un-needed pointer to NULL
		list1Ptr = NULL;
		freelistPtr = NULL;

		counter++;
		sem_post(&list2);//node has been added to list2
		sem_post(&mx);//allow another thread to access the critical section
		
	}

}

void *thread3()
{
	struct listType *list2Ptr = NULL;
	struct listType *freelistPtr = NULL;

	while(counter < limit)
	{
		sem_wait(&list2);
		sem_wait(&mx);


		printf("Thread 3:\n");

		// Print list 2
		printf("Nodes in list 2: ");

		// Set list2Ptr to list2Head
		list2Ptr = list2Head;

		// Iterates through list 2
                while(list2Ptr != NULL)
                {
			// In case node's data is zero, give it another value
			if(list2Ptr->data == 0)
			{
				list2Ptr->data = rand() % 100; //randomizing the data for the node;
			}

                        printf("%d -> ", list2Ptr->data); //printing the data in the free list

                        list2Ptr = list2Ptr->next; //advancing to the next node
                }

                printf("\n");

		//take node from list2 and add it to the free list

		//unlink node from list 2
		list2Ptr = list2Head;
		list2Head = list2Ptr->next;
		list2Ptr->next = NULL;

		//comsume list2Ptr info by setting to zero
		list2Ptr->data = 0;
		printf("Comsuming data on List 2 (setting integer to 0)\n");

		//Links list 2 node to freelist
		if(freeHead == NULL) //Checks if list is empty
		{
			freeHead = list2Ptr;
		}
		else
		{
			list2Ptr->next = freeHead;
			freeHead = list2Ptr;
		}
		
		//set un-needed pointer to NULL
		list2Ptr = NULL;
		
		counter++;
		sem_post(&freeList); //node has been added to the free list
		sem_post(&mx); //allow another thread to access the critical section


	}

}

