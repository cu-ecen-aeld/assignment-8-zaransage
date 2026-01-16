#include <linux/mutex.h>
#include <linux/device.h>

struct aesd_dev {
    struct cdev cdev;

    struct aesd_circular_buffer buffer;
    struct mutex lock;

    char *pending;
    size_t pending_len;

    struct class *class;
    struct device *device;
    dev_t devno;
};
