#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define CLOBALMEM_SIZE 0X1000
#define MEM_CLEAR 0X01
#define GLOBALMEM_MAJOR 230

static int globalmem_major = GLOBALMEM_MAJOR;
module_param(globalmem_major, int ,S_IRUGO);

struct globalmem_dev{
	struct cdev cdev;
	unsigned char mem[CLOBALMEM_SIZE];
};

struct globalmem_dev * globalmem_devp;

//================================================================================================//

static int globalmem_open(struct inode *inode, struct file *filp){
	return 0;
}

static int globalmem_release(struct inode *inode, struct file *filp){
	return 0;
}

static long globalmem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	return 0;
}

static ssize_t globalmem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos){
	return 0;
}

static ssize_t globalmem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos){
	return 0;
}

static loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig){
	return 0;
}

//=================================================================================================//

static const struct file_operations globalmem_fops = {
	.owner = THIS_MODULE,
	.llseek = globalmem_llseek,
	.read = globalmem_read,
	.write = globalmem_write,
	.unlocked_ioctl = globalmem_ioctl,
	.open = globalmem_open,
	.release = globalmem_release
};

static void globalmem_setup_cdev(struct globalmem_dev * dev, int index){

}

static int __init globalmem_init(void){
	return 0;
}

module_init(globalmem_init);

static void __exit globalmem_exit(void){

}

module_exit(globalmem_exit);

MODULE_AUTHOR("ZHUXC <zhuxc@kernel.org>");
MODULE_LICENSE("GPL v2");

