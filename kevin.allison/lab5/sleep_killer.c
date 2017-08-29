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

#define SIGKILL     9

/* Global task structure */
struct task_struct *ts;

int init(void);
void cleanup(void);

int thread(void *data)
{
	struct task_struct *task;

	while(1) {
		for_each_process(task) {
			/* find a process named 'gedit' */
			if (strcmp("gedit", task->comm) == 0) {
                struct task_struct *tmp_task;
                struct pid *vpid;
                int err;

				printk("%s[%d]\n", task->comm, task->pid);

				/* trace process back to init, print results */
				for (tmp_task = task; tmp_task != &init_task; tmp_task = tmp_task->parent) {
					printk(KERN_INFO "--> %s\n", tmp_task->comm);
				}
                printk(KERN_INFO "--> %s\n", tmp_task->comm);

				/* force kill the gedit process */
                rcu_read_lock();
                vpid = find_vpid(task->pid);
                rcu_read_unlock();

                if (vpid) 
                    err = kill_pid(vpid, SIGKILL, 1);

				if (!err) {
                    printk(KERN_INFO "gedit_killer killed gedit [%d]\n", task->pid);
                } else {
                    printk(KERN_INFO "gedit_killer failed to kill gedit [%d]\n", task->pid);
                }
			}
		}

		/* make sure we sleep here to yield the CPU, or we hang the system */
		msleep(100);

		/* time to exit? */
		if (kthread_should_stop())
			break;
	}

	return 0;
}


int init(void)
{
	printk(KERN_INFO "Loding sleep_killer...\n");
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

