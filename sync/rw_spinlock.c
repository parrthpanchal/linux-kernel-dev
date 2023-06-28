#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#define MOD_NAME "[RW_SPINLOCK_SYNC]"

struct task_struct *rth1,*rth2,*wth;
unsigned int c;

DEFINE_RWLOCK(lock);

int reader(void* data) {

	while(!kthread_should_stop()) {
		read_lock(&lock);
		pr_info("%s:counter: %d\n",(char*)data, c);
		read_unlock(&lock);
		msleep(500);
	}

	return 0;
}


int writer(void* data) {

	while(!kthread_should_stop()) {
		write_lock(&lock);
		c++;
		write_unlock(&lock);
		msleep(500);
	}

	return 0;
}


static int __init my_mod_init(void) {

	pr_info("%s Entering Module",MOD_NAME);
	c = 0;
	rth1 = kthread_run(reader,"RT1","Read thread 1");
	rth2 = kthread_run(reader,"RT2","Read thread 2");
	wth = kthread_run(writer,NULL,"Write thread");
	return 0;
}

static void __exit my_mod_exit(void) {
	kthread_stop(rth1);
	kthread_stop(rth2);
	kthread_stop(wth);
	pr_info("%s Exiting module.\n",MOD_NAME);
}


module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_AUTHOR("Parth Panchal");
MODULE_DESCRIPTION("rw_spinlock");
MODULE_LICENSE("Dual MIT/GPL");