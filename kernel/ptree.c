#include <linux/unistd.h>
#include <linux/kernel.h>

asmlinkage int sys_ptree() {
    return printk("Hello, world!\n");
}
