#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana");
MODULE_DESCRIPTION("Driver Prototype");
MODULE_VERSION("0.1");

#define DRIVER_NAME "prototype_driver"
#define BUF_SIZE    256

struct my_device_data {
    struct cdev cdev;
    char buffer[BUF_SIZE];
    size_t len;             
};

static dev_t my_dev_number;
static struct class *my_class;
static struct my_device_data my_device;

static int     my_open(struct inode *inode, struct file *file);
static int     my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos);
static ssize_t my_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos);

static const struct file_operations my_file_ops = {
    .owner   = THIS_MODULE,
    .open    = my_open,
    .release = my_release,
    .read    = my_read,
    .write   = my_write,
};

static int my_open(struct inode *inode, struct file *file)
{
    struct my_device_data *data;

    data = container_of(inode->i_cdev, struct my_device_data, cdev);
    file->private_data = data;
    pr_info(DRIVER_NAME ": open\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info(DRIVER_NAME ": release\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    struct my_device_data *data = file->private_data;
    size_t avail;
    size_t to_copy;

    if (*ppos >= data->len)
        return 0; 

    avail = data->len - *ppos;
    to_copy = (count < avail) ? count : avail;

    if (copy_to_user(ubuf, data->buffer + *ppos, to_copy))
        return -EFAULT;

    *ppos += to_copy;
    return to_copy;
}

static ssize_t my_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos)
{
    struct my_device_data *data = file->private_data;
    size_t space;
    size_t to_copy;

    space = BUF_SIZE - data->len;
    if (space == 0)
        return -ENOSPC;

    to_copy = (count < space) ? count : space;

    if (copy_from_user(data->buffer + data->len, ubuf, to_copy))
        return -EFAULT;

    data->len += to_copy;
    return to_copy;
}

static int __init my_driver_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&my_dev_number, 0, 1, DRIVER_NAME);
    if (ret < 0) {
        pr_err(DRIVER_NAME ": alloc_chrdev_region failed: %d\n", ret);
        return ret;
    }

    cdev_init(&my_device.cdev, &my_file_ops);
    my_device.cdev.owner = THIS_MODULE;
    my_device.len = 0;

    ret = cdev_add(&my_device.cdev, my_dev_number, 1);
    if (ret < 0) {
        pr_err(DRIVER_NAME ": cdev_add failed: %d\n", ret);
        unregister_chrdev_region(my_dev_number, 1);
        return ret;
    }

    my_class = class_create(THIS_MODULE, DRIVER_NAME);
    if (IS_ERR(my_class)) {
        ret = PTR_ERR(my_class);
        pr_err(DRIVER_NAME ": class_create failed: %d\n", ret);
        cdev_del(&my_device.cdev);
        unregister_chrdev_region(my_dev_number, 1);
        return ret;
    }

    if (IS_ERR(device_create(my_class, NULL, my_dev_number, NULL, DRIVER_NAME))) {
        pr_err(DRIVER_NAME ": device_create failed\n");
        class_destroy(my_class);
        cdev_del(&my_device.cdev);
        unregister_chrdev_region(my_dev_number, 1);
        return -ENOMEM;
    }

    pr_info(DRIVER_NAME ": loaded (major=%d minor=%d)\n",
            MAJOR(my_dev_number), MINOR(my_dev_number));
    return 0;
}

static void __exit my_driver_exit(void)
{
    device_destroy(my_class, my_dev_number);
    class_destroy(my_class);
    cdev_del(&my_device.cdev);
    unregister_chrdev_region(my_dev_number, 1);
    pr_info(DRIVER_NAME ": unloaded\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);
