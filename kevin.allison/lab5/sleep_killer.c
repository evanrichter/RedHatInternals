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

	while(1) {
		for_each_process(task) {
			/* find a process named 'gedit' */
			if (strcmp("gedit", task->comm) == 0) {
				printk("%s[%d]\n", task->comm, task->pid);

				/* trace process back to init, print results */
				for (tmp_task = current; tmp_task != &init_task; tmp_task = tmp_task->parent) {
					printk(KERN_INFO "--> %s\n", tmp_task->comm);
				}

				/* force kill the gedit process */
				force_sig(9, task);
				set_task_state(task, TASK_STOPPED);
				printk(KERN_INFO "gedit_killer LKM killed gedit [%d]\n", task->pid);
			}
		}

		/* make sure we sleep here to yield the CPU, or we hang the system */
		msleep(1000);

		/* time to exit? */
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
   printk(KERN_ALERT "Unloading sleep_killer ...\n");

   /* signal the thread to stop */
   kthread_stop(ts);
}

module_init(init);
module_exit(cleanup);

