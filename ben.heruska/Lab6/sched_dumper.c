/* focalpoint LKI */
/* Lab6: Scheduler Lab */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/string.h>

//#include <asm/string.h>
#define DRIVER_AUTHOR "focalpoint"
#define DRIVER_DESC   "Lab6"

MODULE_LICENSE("GPL");           // Get rid of taint message by declaring code as GPL.

/*  Or with defines, like this: */
MODULE_AUTHOR(DRIVER_AUTHOR);    // Who wrote this module?
MODULE_DESCRIPTION(DRIVER_DESC); // What does this module do?

struct task_struct *ts;

int init(void);
void cleanup(void);

struct sched_param s_param;

int thread(void *data)
{
	struct task_struct *task;
	int last_pid = 0;

	while(1)
	{
		for_each_process(task)
		{
			/* find setsched process and save PID */
			printk("Task %s (pid = %d)\n",task->comm, task_pid_nr(task));

			if(strcmp(task->comm,"setsched") == 0){
				printk("Found setsched process, changing scheduling policy and priority level ...\n");
				/* change policy and priority */
				task->static_prio = 100;
				task->policy = SCHED_RR;
			}
		}

		/* yield CPU for other processes */
		msleep(100);

		/* check for pending signal to stop */
		if (kthread_should_stop())
			break;
	}

	return 0;
}


int init(void)
{
	printk(KERN_INFO "init_module() called\n");
	ts = kthread_run(thread, NULL, "kthread");

	return 0;
}

void cleanup(void)
{
   printk(KERN_ALERT "Unloading sched_dumper ...\n");

   /* stop thread */
   kthread_stop(ts);
}

module_init(init);
module_exit(cleanup);

