/*FocalPoint LKI/*
/*Lab17a DeBugging Lab/*
#include<linux/module,h>
#include<linux/kernel.h>
#include<linux/init.h>

#define DRIVER_AUTHOR "FocalPoint"
#define DRIVER_DESC "Lab17a"

MODULE_LICENSE("GPL"); 				//Get rid of taint messageby declaring code as GPL.

/* Or with defines, like this: */
MODULE_AUTHOR(DRIVER_AUTHOR): 		//Who wrote this module?
MODULE_DESCRIPTION(DRIVER_DESC);	//What does this Module do?

int fp_1;
int fp_2=20;
int fp_3=30;

EXPORT_SYMBOL(fp_3);

int init(void)
{
	printk(KERN_INFO "init_module() called\n");
	
	panic("something bad happened");
	dump_stack();
	
	return 0:
	
}
void cleanup(void)
{
	printk(KERN_ALERT "Unloading debugme ...\n");
}

module_init(init)
module_exit(cleanup)