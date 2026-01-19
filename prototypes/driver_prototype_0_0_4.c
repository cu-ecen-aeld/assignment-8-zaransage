#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/sched.h>  
#include <linux/kernel.h> 
#include <linux/types.h>  
#include <linux/completion.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana");
MODULE_DESCRIPTION("Another prototype Driver");


#define DEVICE_NAME "prototype_four"
#define BUFFER_LEN 256

static int major;
static char message[BUFFER_LEN];

static int my_open(struct inode *inode, struct file *file){
    printk(KERN_DEBUG "Opening file\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file){
    printk(KERN_DEBUG "Releaseing file\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset){
    size_t message_length = strlen(message);
    if (*offset >= message_length){
        return 0;
    }

    if (copy_to_user(buf, message + *offset, message_length - *offset)){
        return -EFAULT;
    }

    *offset = message_length;
    return message_length;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset){
    if (len > BUFFER_LEN){
        return -EINVAL;
    }

    if (copy_from_user(message, buf, len)){
        return -EFAULT;
    }

    message[len] = '\0';
    printk(KERN_DEBUG "got %zu bytes: %s\n", len, message);
    return len;

}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

static int __init my_driver_init(void){
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0){
        printk(KERN_DEBUG "Failed to register device\n");
        return major;
    }

    printk(KERN_DEBUG "Registered with major number %d\n", major);
    return 0;
}

static void __exit my_driver_exit(void){
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_DEBUG "Unregistered device\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);