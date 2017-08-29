/* focalpoint LKI */
/* Lab11: Signals Lab */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/delay.h>
#include <linux/kthread.h>

#include <asm/signal.h>
#include <linux/delay.h>

#define DRIVER_AUTHOR   "focalpoint"
#define DRIVER_DESC     "Lab11"

#define KERNEL_SIG      ((struct siginfo*)1) 

#define SLEEP_FOR(x)    do { \
                            set_current_state(TASK_INTERRUPTIBLE); \
                            schedule_timeout(x * HZ); \
                            set_current_state(TASK_RUNNING); \
                        } while(0);

MODULE_LICENSE("GPL");           // Get rid of taint message by declaring code as GPL.

/*  Or with defines, like this: */
MODULE_AUTHOR(DRIVER_AUTHOR);    // Who wrote this module?
MODULE_DESCRIPTION(DRIVER_DESC); // What does this module do?

struct task_struct *ts;

int init(void);
void cleanup(void);

int thread(void *data)
{
	struct task_struct *task;

	while(1) {
		for_each_process(task) {

            /* Look for the siguser process */
            if (strcmp("siguser", task->comm) == 0) {
                printk("signalex: Found siguser process [%d], sending signals\n", task->pid);

                send_sig_info(SIGABRT, KERNEL_SIG, task);
                SLEEP_FOR(2);
                send_sig_info(SIGTRAP, KERNEL_SIG, task);
                SLEEP_FOR(2);
                send_sig_info(SIGQUIT, KERNEL_SIG, task);

                /* terminate the process after 5 seconds*/
                SLEEP_FOR(5);
                send_sig_info(SIGKILL, KERNEL_SIG, task);
            }
		}

		msleep(100);
		if (kthread_should_stop())
			break;
	}

	return 0;
}


int init(void)
{
	printk(KERN_INFO "signalex: init_module() called\n");
	ts = kthread_run(thread, NULL, "kthread");

	return 0;
}

void cleanup(void)
{
   printk(KERN_ALERT "Unloading signalex ...\n");
   kthread_stop(ts);
}

module_init(init);
module_exit(cleanup);

