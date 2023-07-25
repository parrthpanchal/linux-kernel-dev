#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/sched.h>


#define MOD_NAME "[SEMAPHORE_SYNC]: "
#define LOG_INFO(str,...) pr_info(MOD_NAME str __VA_OPT__(,) __VA_ARGS__)

struct priv_data {
	int a;
	int b;
};


struct priv_data * p_data;
struct task_struct* t_reader,*t_writer;

DEFINE_SEMAPHORE(semlock);

void data_init(struct priv_data* p) {

	p->a = 0;
	p->b = 0;
}


int reader_thread(void* data) {

	int rc;
	LOG_INFO("reader thread");
	// Attempting to lock
	rc = down_interruptible(&semlock);
	if(rc  < 0) return -EINTR;
	LOG_INFO(" a = %d b = %d \n",p_data->a,p_data->b);
	up(&semlock);
	return 0;
}

int writer_thread(void* data) {

	LOG_INFO("writer thread");
	down_interruptible(&semlock);
	p_data->a = 10;
	p_data->b = 20;
	up(&semlock);
	return 0;
}


static int __init module_init_routine(void) {

	LOG_INFO("Entering Module\n");

	p_data = (struct priv_data*)kmalloc(sizeof(struct priv_data),GFP_KERNEL);
	data_init(p_data);

	t_writer = kthread_run(writer_thread,"WT","writer_thread");
	t_reader = kthread_run(reader_thread,"RT","reader_thread");

	return 0;

}


static void __exit module_exit_routine(void) {

	kfree(p_data);
	LOG_INFO("Exiting Module\n");

}


module_init(module_init_routine);
module_exit(module_exit_routine);

MODULE_AUTHOR("Parth Panchal");
MODULE_DESCRIPTION("Semaphore Sync");
MODULE_LICENSE("GPL");