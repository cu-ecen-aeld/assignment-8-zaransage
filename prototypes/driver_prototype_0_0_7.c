#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/sched.h>  
#include <linux/kernel.h> 
#include <linux/types.h>  
#include <linux/completion.h>
#include <linux/slab.h>
#include "aesd-circular-buffer.h"
#include "aesd-circular-buffer.c"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana");
MODULE_DESCRIPTION("Another prototype Driver");

#define DEVICE_NAME "prototype_seven"
#define BUFFER_LEN 256

static int major;
static char *message = NULL;
static size_t message_length = 0;

static char *my_stored_memory[10];
static int count = 0;

// Start of my buffer init work

// Initialize my buffer
struct aesd_circular_buffer *myBuffer = malloc(sizeof(struct aesd_circular_buffer)); 

// init said buffer
aesd_circular_buffer_init(myBuffer);

//End of my buffer init work

// Commands for working with the buffer:
// char *data = "Dirka";
// Get Data:
// struct aesd_buffer_entry *entry = malloc(sizeof(struct aesd_buffer_entry));
// entry->buffprt = data; //This reads the data in
// entry->size = strlen(data);

// Define an offset for the entry:
// int offset = 4;
// printf("Char %c at offset %d.\n", entry->buffptr[offset], offset);

// Now, if I want to add that entry:
// Specify the buffer to use and the entry.
// aesd_circular_buffer_add_entry(myBuffer. entry);

// If I want to find an entry or position:
// size_t char_offset = 1;
// size_t *entry_offset_byte_rtn;

// Find and get the entry:
// aesd_circular_buffer_find_entry_offset_for_fpos(my_buffer, char_offset, entry_offset_byte_rtn);

// I need to remember to free the data after.
// free(entry);
// free(myBuffer);

// The start of file operations
static int my_open(struct inode *inode, struct file *file){
    printk(KERN_DEBUG "Opening file\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file){
    printk(KERN_DEBUG "Releaseing file\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset){
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
    
    char *new_buffer;
    int i;

    new_buffer = kmalloc(len +1, GFP_KERNEL);
    if (!new_buffer){
        return -ENOMEM;
    }

    my_stored_memory[(ssize_t)count] = new_buffer;

    if (copy_from_user(new_buffer, buf, len)){
        kfree(new_buffer);
        return -EFAULT;
    }

    new_buffer[len] = '\0';

    if (count > 10){
        kfree(my_stored_memory[0]);
        for (i = 0; i < 10 -1; i++){
            my_stored_memory[i] = my_stored_memory[i+1];
        }
        count = 10 - 1;
    }

    my_stored_memory[count] = new_buffer;
    count++;

    message = new_buffer;
    message_length = len;

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
    if (message){
        kfree(message);
    }
    
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_DEBUG "Unregistered device\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

// To test, run insmod <driver_name> then find the major number in dmesg. Then mknod -m 666 /dev/prototype_six c 240 0 - where 240 is the real major number
// Next step, work on reading from buffer correctly from user space.
// Major minor number dymanic code mostly from here: https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html