#include <stdio.h>
#include <unistd.h>
#include <linux/sched.h>
#include <sys/resource.h>

/*
 * Scheduling policies defined in sched.h
#define SCHED_NORMAL	0
#define SCHED_FIFO		1
#define SCHED_RR		2
#define SCHED_BATCH		3
 */

int main()
{

	while(1)
	{
		printf("PID %i sleeping..\n", getpid());
		switch(sched_getscheduler(getpid()))
		{
			case SCHED_NORMAL: printf("sched_getscheduler() = SCHED_NORMAL\n");
								break;
			case SCHED_FIFO: printf("sched_getscheduler() = SCHED_FIFO\n");
								break;
			case SCHED_RR: printf("sched_getscheduler() = SCHED_RR\n");
								break;
			case SCHED_BATCH: printf("sched_getscheduler() = SCHED_BATCH\n");
								break;
			default:
				break;
		}

		printf("Process priority %i...\n", getpriority(PRIO_PROCESS, 0));
		sleep(5);
	}
}
