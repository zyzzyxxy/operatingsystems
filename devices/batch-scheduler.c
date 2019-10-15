/* Tests cetegorical mutual exclusion with different numbers of threads.
 * Automatic checks only catch severe problems like crashes.
 */
#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "lib/random.h" //generate random numbers

#define BUS_CAPACITY 3
#define SENDER 0
#define RECEIVER 1
#define NORMAL 0
#define HIGH 1

/*
 *	initialize task with direction and priority
 *	call o
 * */
typedef struct {
	int direction;
	int priority;
} task_t;

struct semaphore task_send_prio;
struct semaphore task_receive_prio;
struct semaphore packets_in_bus;
struct lock blocked;
int currentDir;
int bussSlotsFree;

void batchScheduler(unsigned int num_tasks_send, unsigned int num_task_receive,
        unsigned int num_priority_send, unsigned int num_priority_receive);

void senderTask(void *);
void receiverTask(void *);
void senderPriorityTask(void *);
void receiverPriorityTask(void *);


void oneTask(task_t task);/*Task requires to use the bus and executes methods below*/
	void getSlot(task_t task); /* task tries to use slot on the bus */
	void transferData(task_t task); /* task processes data on the bus either sending or receiving based on the direction*/
	void leaveSlot(task_t task); /* task release the slot */



/* initializes semaphores */ 
void init_bus(void){ 
 
    random_init((unsigned int)123456789); 
    sema_init(&task_send_prio, 0);
    sema_init(&task_receive_prio, 0);
    sema_init(&packets_in_bus, 0);
    lock_init(&blocked);
    currentDir = SENDER;
    bussSlotsFree = BUS_CAPACITY;

}

/*
 *  Creates a memory bus sub-system  with num_tasks_send + num_priority_send
 *  sending data to the accelerator and num_task_receive + num_priority_receive tasks
 *  reading data/results from the accelerator.
 *
 *  Every task is represented by its own thread. 
 *  Task requires and gets slot on bus system (1)
 *  process data and the bus (2)
 *  Leave the bus (3).
 */

void batchScheduler(unsigned int num_tasks_send, unsigned int num_task_receive,
        unsigned int num_priority_send, unsigned int num_priority_receive)
{
	while(num_tasks_send>0)
	{
		thread_create("numtasksend", 0, senderTask , NULL);
		num_tasks_send--;
	
	}
	while(num_task_receive>0){
		thread_create("numRecieve", 0,receiverTask , NULL);
		num_task_receive--;
	}
	while(num_priority_send>0){
		thread_create("PrioritySend", 0,senderPriorityTask , NULL);
		num_priority_send--;
	}
	while(num_priority_receive>0)
	{
		thread_create("PriorityRecieve", 0, receiverPriorityTask, NULL);
		num_priority_receive--;
	}


}

/* Normal task,  sending data to the accelerator */
void senderTask(void *aux UNUSED){
        task_t task = {SENDER, NORMAL};
        oneTask(task);
}

/* High priority task, sending data to the accelerator */
void senderPriorityTask(void *aux UNUSED){
        task_t task = {SENDER, HIGH};
        oneTask(task);
}

/* Normal task, reading data from the accelerator */
void receiverTask(void *aux UNUSED){
        task_t task = {RECEIVER, NORMAL};
        oneTask(task);
}

/* High priority task, reading data from the accelerator */
void receiverPriorityTask(void *aux UNUSED){
        task_t task = {RECEIVER, HIGH};
        oneTask(task);
}

/* abstract task execution*/
void oneTask(task_t task) {
  getSlot(task);
  transferData(task);
  leaveSlot(task);
}


/* task tries to get slot on the bus subsystem */
void getSlot(task_t task) 
{
	lock_acquire(&blocked);
	if(task.priority == HIGH){
	   if(task.direction == SENDER){
		sema_up(&task_send_prio);
		}
	   else{
		sema_up(&task_receive_prio);
	    }
	}
	while(1){
		if(task.priority == HIGH){
		    if((bussSlotsFree>0 && task.direction == currentDir)||bussSlotsFree == 3){
				bussSlotsFree--;
				currentDir = task.direction;
			break;
			}
			printf("IN HIGH rec send:  %d%d\n", task_receive_prio.value, task_send_prio.value);	
		}
		else{//low prio
			if(task_send_prio.value==0 && task_receive_prio.value==0){
			   if((task.direction == currentDir && bussSlotsFree>0)||bussSlotsFree == 3){
				bussSlotsFree--;
				currentDir = task.direction;
				break;
				}
			}
		

			printf("IN LOW rec send taskDir currentDir bussSlotFree:  %d %d %d% d% d\n", task_receive_prio.value, task_send_prio.value, task.direction, currentDir, bussSlotsFree);	
			printf("IN LOW packets in bus:  %d\n", packets_in_bus.value);	
			/*
		    if(task.direction == currentDir && bussSlotsFree>0){
			if(task_send_prio.value==0 && task_receive_prio.value==0){
			printf("break3 rec send:  %d%d\n", task_receive_prio.value, task_send_prio.value);	
				break;
			}	
			}
			if(bussSlotsFree==3 && task_send_prio.value==0 && task_receive_prio.value==0){
			printf("break4 rec send:  %d%d\n", task_receive_prio.value, task_send_prio.value);	
				break;	
			}*/
		}
	
	}
	sema_up(&packets_in_bus);
	lock_release(&blocked);
}

/* task processes data on the bus send/receive */
void transferData(task_t task) 
{
   printf("Enter transfer data\n");
	timer_sleep(random_ulong()%25);  
	printf("Exit transfer data\n");
    /* FIXME implement */
}

/* task releases the slot */
void leaveSlot(task_t task) 
{

	if(task.priority == HIGH){
	   if(task.direction == SENDER){
		sema_down(&task_send_prio);
		}
	   else{
		sema_down(&task_receive_prio);
	    }
	}
	
    bussSlotsFree++;
	sema_down(&packets_in_bus);
    /* FIXME implement */
}
