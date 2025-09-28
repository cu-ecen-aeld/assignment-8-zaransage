#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prototype from pyjamabrah.com with my edits");
MODULE_DESCRIPTION("Basic driver example I am learning to help");


static struct proc_dir_entry * custom_proc_node;

static ssize_t pyjama_read(struct file *file_pointer, char *use_space_buffer, size_t count, loff_t* offset){
    printk("pyjama_read\n");
    return 0;
}

struct proc_ops driver_proc_ops = {
    .proc_read = pyjama_read
}

static int pyjama_module_init(void){
    printk("Pyjama_module_init: entry\n");

    custom_proc_node = proc_create("pyjama_driver", 0, NULL, &driver_proc_ops);

    if (custom_proc_node == NULL) {
        printk("pyjama_module_init: error\n");
        return -1;
    }

    printk("pykama_module_init: exit\n");
    return 0;
}

static void pyjama_module_exit(void){
    printk("pyjama_module_exit: entry\n");

    proc_remove(custom_proc_node);

    printk("pyjama_module_exit: exit\n");

}

module_init(pyjama_module_init);
module_exit(pyjama_module_exit);