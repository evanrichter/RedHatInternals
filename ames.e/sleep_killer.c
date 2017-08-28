/* focalpoint LKI */
/* Lab5: Process Management Lab */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/delay.h>
#include <linux/kthread.h>

//#include <asm/string.h>
#define DRIVER_AUTHOR "focalpoint"
#define DRIVER_DESC   "Lab5"

MODULE_LICENSE("GPL");           // Get rid of taint message by declaring code as GPL.

/*  Or with defines, like this: */
MODULE_AUTHOR(DRIVER_AUTHOR);    // Who wrote this module?
MODULE_DESCRIPTION(DRIVER_DESC); // What does this module do?

/* Global task structure */
struct task_struct *ts;

int init(void);
void cleanup(void);

int thread(void *data)
{
	struct task_struct *task, *tmp_task;
	int last_pid = 0;

	while(1)
	{
		for_each_process(task)
		{
			/* find a process named 'sleep' */
			printk("%s[%d]\n", task->comm, task->pid);
			
			if (strcmp(task->comm,"gedit") == 0)	//We found a gedit
			{
				printk("Found a sleep process: [%d}\n", task->pid);
				/* if found, save the PID */
				
				last_pid = task->pid;	//save you for later
				tmp_task = task;
				
				/* trace process back to init, print results */
			
				if (tmp_task != &init_task) tmp_task = tmp_task->parent;	//go up one level
				/*{
					printk(KERN_INFO "  Process trace: %s [%d]", tmp_task->comm, tmp_task->pid);
				}
			*/

				/* force kill the sleeping process */
				force_sig(9, task);
				set_task_state(tmp_task, TASK_STOPPED);
				printk(KERN_INFO "sleep_killer LKM killed gedit parent process [%d]\n", task->pid);
			}
		}

		if(kthread_should_stop()) break;
		msleep(100);
	}
		

		
	kthread_should_stop();
	

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
   printk(KERN_ALERT "Unloading sleep_killer ...\n");

   /* signal the thread to stop */
   kthread_stop(ts);
}

module_init(init);
module_exit(cleanup);

