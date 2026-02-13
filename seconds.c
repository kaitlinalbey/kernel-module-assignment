#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

static unsigned long start_jiffies;

ssize_t proc_read(struct file *file, char __user *usr_buf,
                  size_t count, loff_t *pos);

static struct proc_ops proc_ops = {
    .proc_read = proc_read,
};

int proc_init(void)
{
    start_jiffies = jiffies;
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    printk(KERN_INFO "/proc/seconds created\n");
    return 0;
}

void proc_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/seconds removed\n");
}

ssize_t proc_read(struct file *file, char __user *usr_buf,
                  size_t count, loff_t *pos)
{
    char buffer[BUFFER_SIZE];
    int len;
    unsigned long elapsed;

    if (*pos > 0)
        return 0;

    elapsed = (jiffies - start_jiffies) / HZ;

    len = snprintf(buffer, BUFFER_SIZE, "%lu\n", elapsed);

    if (copy_to_user(usr_buf, buffer, len))
        return -EFAULT;

    *pos = len;
    return len;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("Kaitlin");