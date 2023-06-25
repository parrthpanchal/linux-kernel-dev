#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/types.h>

#define MOD_NAME "SPINLOCK_SYNC"

struct data{
	char c;
	spinlock_t  lock;
}; 

struct data* p;
struct task_struct *read_th,*write_th;


int read_thread(void* data) {

		pr_info("[%s]: Trying to acquire lock in read_thread",MOD_NAME);
		spin_lock(&p->lock);
		pr_info("[%s]: Data : %c",MOD_NAME,p->c);
		spin_unlock(&p->lock);
		return 0;
}

int write_thread(void* data) {

		spin_lock(&p->lock);
		p->c = 'B';
		spin_unlock(&p->lock);
		return 0;
}

int module_init_routine(void) {

	pr_info("[%s]: Entering module",MOD_NAME);
	p = (struct data *)kmalloc(sizeof(struct data),GFP_KERNEL);

	if(p == NULL) {
		pr_err("Unable to allocate memory");
		return -1;
	}

	p->c = 'A';
	spin_lock_init(&p->lock);

	write_th = kthread_run(write_thread,NULL,"write-thread");

	if(write_th == NULL) {
		pr_info("[%s]: Unable to create write thread",MOD_NAME);
		return -1;
	}

	read_th = kthread_run(read_thread,NULL,"read_thread");

	if(read_th == NULL) {
		pr_info("[%s]: Unable to create read thread",MOD_NAME);
		return -1;
	}

	return 0;
}


void module_exit_routine(void) {

	pr_info("[%s]: Exiting module",MOD_NAME);
	kfree(p);

}


module_init(module_init_routine);
module_exit(module_exit_routine);

MODULE_AUTHOR("Parth Panchal");
MODULE_DESCRIPTION("Spinlock Sync");
MODULE_LICENSE("GPL");