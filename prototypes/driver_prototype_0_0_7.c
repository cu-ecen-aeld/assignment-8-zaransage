#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/sched.h>  
#include <linux/kernel.h> 
#include <linux/types.h>  
#include <linux/completion.h>
#include <linux/slab.h>
//#include "aesd-circular-buffer.h"
#include "aesd-circular-buffer.c"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana");
MODULE_DESCRIPTION("Another prototype Driver");

#define DEVICE_NAME "prototype_seven"
#define BUFFER_LEN 256

static int major;
static char *message = NULL;

//static char *my_stored_memory[10];
//static int count = 0;

struct aesd_circular_buffer *myBuffer;

// Start of my buffer init work

static int setup_buffer_items(void){

    // Allocate memory for my buffer
    myBuffer = kmalloc(sizeof(struct aesd_circular_buffer), GFP_KERNEL);
    
     if (!myBuffer) {
        return -ENOMEM;
    }

    // init said buffer
    aesd_circular_buffer_init(myBuffer);

    return 0; 
}

//End of my buffer init work

// Commands for working with the buffer:
// char *data = "Dirka";
// Get Data:
// struct aesd_buffer_entry *entry = kmalloc(sizeof(struct aesd_buffer_entry));
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

    struct aesd_buffer_entry *entry;
    size_t entry_offset_byte = 0;
    size_t bytes_to_copy;
    size_t total_copied = 0;

    if (len == 0) {
        return 0;
    }

    if (!myBuffer) {
        return -ENODEV;
    }

    while (total_copied < len){

        entry = aesd_circular_buffer_find_entry_offset_for_fpos(myBuffer, *offset + total_copied, &entry_offset_byte);

        if (!entry) {
            break;
        }

        bytes_to_copy = min(len - total_copied, entry->size - entry_offset_byte);

        if (copy_to_user(buf + total_copied, entry->buffptr + entry_offset_byte, bytes_to_copy)){
            return -EFAULT;
         }


    }

    *offset += total_copied;

    return total_copied;


}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    char *kdata;
    struct aesd_buffer_entry add_entry;
    size_t i;

    if (!myBuffer || len == 0)
        return -EINVAL;

    kdata = kmalloc(len + 1, GFP_KERNEL);
    if (!kdata)
        return -ENOMEM;

    if (copy_from_user(kdata, buf, len)) {
        kfree(kdata);
        return -EFAULT;
    }
    kdata[len] = '\0';  // null terminate

    // Look for newline
    for (i = 0; i < len; i++) {
        if (kdata[i] == '\n') {
            // Found complete command: include the \n
            add_entry.buffptr = kdata;
            add_entry.size = i + 1;

            // Add to circular buffer (may evict old entry)
            aesd_circular_buffer_add_entry(myBuffer, &add_entry);

            printk(KERN_INFO "prototype_seven: stored command: %.*s", (int)add_entry.size, add_entry.buffptr);
            return len;  // success: full line written
        }
    }

    // No newline → partial command
    // For full aesdchar, we buffer it in file->private_data
    // But for now, reject partial writes (simpler and acceptable)
    printk(KERN_WARNING "prototype_seven: partial write rejected (no newline)\n");
    kfree(kdata);
    return -EINVAL;
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

    setup_buffer_items();

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