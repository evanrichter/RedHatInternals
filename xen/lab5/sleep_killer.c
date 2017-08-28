/* focalpoint LKI */
/* Lab5: Process Management Lab */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/string.h> // ADDED BY STUDENT
#include <linux/signal.h>

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
	struct siginfo sigkill;
	int errkill;

	sigkill.si_signo = SIGKILL;
	sigkill.si_errno = 0;
	sigkill.si_code = SI_QUEUE;
	sigkill.si_pid = current->tgid;



	while(1)
	{
		for_each_process(task)
		{
			

			if (strncmp(task->comm, "gedit", 5) == 0) // WAS task->comm == "sleep" // use 5 for length of sleep ( excl \0 )
			{
				struct pid *vpid;
				/* find a process named 'sleep' */
				printk("%s[%d]\n", task->comm, task->pid);
				/* if found, save the PID */
				last_pid = task->pid;
				
				/* trace process back to init, print results */
				for (tmp_task = task; task != &init_task; task = task->parent) { //task = current
					printk("  < %s-%d ", task->comm, task->pid);
				}
				printk("  < %s-%d\n", task->comm, task->pid);

				task = tmp_task;
				rcu_read_lock();
				vpid = find_vpid(task->pid);
				errkill = kill_pid(vpid, SIGKILL, 1);
				rcu_read_unlock();
				//send_sig_info(SIGKILL, &sigkill, task); // do this, or the above 4 lines
				//printk(KERN_INFO "found process %s\n", task->comm);
				//set_task_state(task, TASK_STOPPED);
				if (errkill == 0) {
					printk(KERN_INFO "sleep_killer LKM killed(%d) %s [%d]\n", errkill, task->comm, task->pid);
				} else {
					printk(KERN_INFO "sleep_killer LKM could not kill %s [%d]\n", task->comm, task->pid);
				}
			}
		}

		/* make sure we sleep here to yield the CPU, or we hang the system */
		msleep(100);

		/* time to exit? */
		//kthread_should_stop();
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

