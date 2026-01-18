/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> 
#include "aesdchar.h"
int driver_major =   0;
int driver_minor =   0;

MODULE_AUTHOR("Dana Marble"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

static aesd_setup_cdev(struct aesd_device *device){
    int error;
    int deviceNumber = MKDEV(driver_major, driver_minor);

    cdev_init(&device->cdev, &aesd_fops);

    device->cdev.owner = THIS_MODULE;

    device->cdev.ops = &aesd_fops;

    error = cdev_add(&device->cdev, deviceNumber, 1);
    if (error) {
        printk(KERN_ERR "There is an error adding the cdev: %d\n", error);
    }

    return error;
}

int aesd_open(struct inode *inode, struct file *filp)
{
    PDEBUG("open");
    /**
     * TODO: handle open
     */

    filp->private_data = container_of(inode->i_cdev, struct aesd_dev, cdev);

    return 0;

     printk("Opened the file!\n");
    return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");
    /**
     * TODO: handle release
     */
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
 
    struct aesd_dev *dev = filp->private_data;
    struct aesd_buffer_entry *entry;
    size_t entry_off = 0;
    size_t to_copy;

    if (count == 0) return 0;

    if (mutex_lock_interruptible(&dev->lock))
        return -ERESTARTSYS;

    while (retval < count) {
        entry = aesd_circular_buffer_find_entry_offset_for_fpos(
            &dev->buffer, *f_pos + retval, &entry_off);

        if (!entry)
            break; // EOF

        to_copy = min(count - retval, entry->size - entry_off);

        if (copy_to_user(buf + retval, entry->buffptr + entry_off, to_copy)) {
            mutex_unlock(&dev->lock);
            return -EFAULT;
        }

        retval += to_copy;
    }

    *f_pos += retval;
    mutex_unlock(&dev->lock);
    return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = -ENOMEM;
    PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle write
     */
    return retval;
}

struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
    .read =     aesd_read,
    .write =    aesd_write,
    .open =     aesd_open,
    .release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
    int err, devno = MKDEV(aesd_major, aesd_minor);

    cdev_init(&dev->cdev, &aesd_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &aesd_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding aesd cdev", err);
    }
    return err;
}

int aesd_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, aesd_minor, 1,
            "aesdchar");
    aesd_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", aesd_major);
        return result;
    }
    memset(&aesd_device,0,sizeof(struct aesd_dev));

    /**
     * TODO: initialize the AESD specific portion of the device
     */

     aesd_circular_buffer_init(&aesd_device.buffer);

     mutex_init(&aesd_device.lock);

     aesd_device.pending = NULL;
     aesd_device.pending_len = 0;


    result = aesd_setup_cdev(&aesd_device);

    if( result ) {
        unregister_chrdev_region(dev, 1);
    }
    return result;

}

void aesd_cleanup_module(void)
{
    dev_t devno = MKDEV(aesd_major, aesd_minor);

    cdev_del(&aesd_device.cdev);

    /**
     * TODO: cleanup AESD specific poritions here as necessary
     */

    unregister_chrdev_region(devno, 1);
}



module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
