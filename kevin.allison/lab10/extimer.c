/* focalpoint LKI */
/* Lab10: Timing Management Lab */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/sched.h>

#define DRIVER_AUTHOR "focalpoint"
#define DRIVER_DESC   "Lab10"

MODULE_LICENSE("GPL");           // Get rid of taint message by declaring code as GPL.

/*  Or with defines, like this: */
MODULE_AUTHOR(DRIVER_AUTHOR);    // Who wrote this module?
MODULE_DESCRIPTION(DRIVER_DESC); // What does this module do?

/* Global timer */
static struct timer_list my_timer;

int init(void);
void cleanup(void);

void my_timer_callback(unsigned long data)
{
	printk("extimer: my_timer_callback called\n");
}

int init(void)
{

	printk(KERN_INFO "init_module() called\n");
	printk("extimer: delaying startup for 10 seconds\n");

    /* set task to interruptible, then sleep
     * for 10 seconds (aka current (jiffies) + 10) */	
	set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout(10 * HZ);
    set_current_state(TASK_RUNNING);


	printk("extimer: setting timer to fire callback in 20 seconds\n");
	/* add a timer to fire my_timer_callback in 20 seconds */
	init_timer(&my_timer);					
	my_timer.expires = jiffies + (20 * HZ);	//Now + 20 seconds from now
	my_timer.data = 0;						//Start at 0 to count
	my_timer.function = my_timer_callback;	//The function to execute

	/* install timer my_timer */
	printk("extimer: installing timer\n");
	add_timer(&my_timer);					//start the timer!
	return 0;
}

void cleanup(void)
{
	/* remove timer */

	printk(KERN_ALERT "Unloading extimer ...\n");
	del_timer(&my_timer);					//Removes the timer
}

module_init(init);
module_exit(cleanup);

