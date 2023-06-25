#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/types.h>

#define MOD_NAME "ATOMIC_SYNC"

atomic_t counter; /* Shared data between threads */
struct task_struct *read_thread,*write_thread;


static int read_function (void* data) {

	while(!kthread_should_stop()) {
		pr_info("[%s] counter : %d\n",MOD_NAME,atomic_read(&counter));
		msleep(500);
	}

	return 0;
}


static int write_function(void* data) {
	
	while(!kthread_should_stop()) {
		atomic_inc(&counter);
		msleep(500);
	}

	return 0;
}

static int __init my_module_init(void) {

	pr_info("[%s] Entry",MOD_NAME);
	atomic_set(&counter,0);

	/* Read thread will atomically read the shared data */
	read_thread = kthread_run(&read_function,NULL,"read-thread");
	/* Write thread will atomically increment the value stored in shared data by 1 */
	write_thread = kthread_run(&write_function,NULL,"write-function");
	return 0;
}


static void __exit my_module_exit(void) {

	pr_info("[%s] Exit",MOD_NAME);
	kthread_stop(read_thread);
	kthread_stop(write_thread);
}


module_init(my_module_init);
module_exit(my_module_exit);


MODULE_AUTHOR("Parth Panchal");
MODULE_DESCRIPTION("synchronization using atomic data type");
MODULE_LICENSE("Dual MIT/GPL");