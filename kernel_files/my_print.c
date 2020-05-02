// #334
#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage void sys_my_print(int pid, long start_time, long end_time){
	static const long BASE = 1000000000;
	printk("[Project1] %d %ld.%09ld %ld.%09ld\n", pid, start_time / BASE, start_time % BASE, end_time / BASE, end_time % BASE);
}
