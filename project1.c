#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

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
	struct listType *freeList = (struct listType *)malloc(sizeof(struct listType));//allocating memory for the free list
	struct listType *list1 = (struct listType *)malloc(sizeof(struct listType));//allocating memory for list1
	struct listType *list2 = (struct listType *)malloc(sizeof(struct listType));//allocating memory for list2
	
	//heads to the linked lists
	struct listType *t1Head = NULL;
	struct listType *t2Head = NULL;
	struct listType *t3Head = NULL;

	pthread_create(&t1, NULL, (void *) &thread1, NULL);
	pthread_create(&t2, NULL, (void *) &thread2, NULL);
	pthread_create(&t2, NULL, (void *) &thread3, NULL);


	//waiting for threads to finish
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);

	//free memory before closing the program
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

