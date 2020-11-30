#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define GLOBALMEM_SIZE 0X1000
#define MEM_CLEAR 0X01
#define GLOBALMEM_MAJOR 230

static int globalmem_major = GLOBALMEM_MAJOR;
module_param(globalmem_major, int ,S_IRUGO);

/* 定义一个设备结构体，包含了对应字符设备的cdev和内存 */
struct globalmem_dev{
	struct cdev cdev;
	unsigned char mem[GLOBALMEM_SIZE];
};

struct globalmem_dev * globalmem_devp;

//================================================================================================//
/* open时会字符设备结构体与file实体中的private_data进行挂钩 */
static int globalmem_open(struct inode *inode, struct file *filp){
	filp->private_data = globalmem_devp;
	return 0;
}

static int globalmem_release(struct inode *inode, struct file *filp){
	return 0;
}

static long globalmem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	struct globalmem_dev *dev = filp->private_data;
	switch(cmd){
	case MEM_CLEAR:
		memset(dev->mem,0,GLOBALMEM_SIZE);
		printk(KERN_INFO "globalmem is set to zero \n");
		break;

	default:
		return -EINVAL;
	}
	return 0;
}

static ssize_t globalmem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos){
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev = filp->private_data;
	if(p>=GLOBALMEM_SIZE){
		return 0;
	}
	if(count > GLOBALMEM_SIZE - p){
		count = GLOBALMEM_SIZE -p;
	}
	if(copy_to_user(buf, dev->mem + p, count)){ /* 由于用户空间不能直接访问内核空间，因此借助该函数实现内核空间到用户空间缓冲区的复制 */
		ret = -EFAULT;
	}else{
		*ppos += count;
		ret = count;
		printk(KERN_INFO "read %u bytes(s) from %lu\n",count,p);
	}
	return ret;
}

static ssize_t globalmem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos){
	unsigned long p = *ppos;
	unsigned int count = size;
	unsigned long ret = 0;
	struct globalmem_dev *dev = filp->private_data;
	if (p >= GLOBALMEM_SIZE){
		return 0;
	}
	if(count > GLOBALMEM_SIZE - p){
		count = GLOBALMEM_SIZE - p;
	}
	if(copy_from_user(dev->mem+p,buf,count)){ /* 类比copy_to_user实现从用户缓冲区到内核空间的复制 */
		ret = -EFAULT;
	}else {
		*ppos += count;
		ret = count;
		printk(KERN_INFO "written %u bytes(s) from %lu\n", count, p);
	}
	return ret;
}

static loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig){
	loff_t ret = 0;
	switch (orig) {
	case 0: /* 从开头惊喜索引 */
		if(offset<0){
			ret = -EINVAL;
			break;
		}
		if(offset>GLOBALMEM_SIZE){
			ret = -EINVAL;
			break;
		}
		filp->f_pos = (unsigned int)offset;
		ret = filp->f_pos;
		break;
	case 1: /* 从当前位置开始索引 */
		if((filp->f_pos+offset) > GLOBALMEM_SIZE){
			ret = -EINVAL;
			break;
		}
		if((filp->f_pos+offset)<0){
			ret = -EINVAL;
			break;
		}
		filp->f_pos += offset;
		ret = filp->f_pos;
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return 0;
}

//=================================================================================================//
/* file_operations结构体中的成员函数是字符设备驱动与内核虚拟文件系统的接口，是用户空间对Linux进行系统调用的最终落实者 */
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
	int err, devno = MKDEV(globalmem_major, index); /* cdev 结构体的dev_t成员定义了设备号，为32位，其中前12位为主设备号后20位位次设备号，该函数用来生成dev_t */
	cdev_init(&dev->cdev, &globalmem_fops); /* cdev_init 用于初始化cdev的成员，并建立cdev和file_operations之间的连接 */
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev,devno,1); /* cdev_add用于向系统中添加一个cdev，对应的cdev_del用于向系统中删除一个cdev */
	if(err){
		printk(KERN_NOTICE "Error %d adding globalmem%d", err,index);
	}
}

static int __init globalmem_init(void){
	int ret;
	dev_t devno = MKDEV(globalmem_major, 0); /* 生成一个设备号 */
	if(globalmem_major){
		ret = register_chrdev_region(devno, 1, "globalmem"); /* 向系统申请指定的设备号 */
	}else{
		ret = alloc_chrdev_region(&devno, 0, 1, "globalmem"); /* 如果指定设备号申请失败，则有系统动态分配一个设备号 */
		globalmem_major = MAJOR(devno);
	}
	if(ret < 0){
		return ret;
	}
	globalmem_devp = kzalloc(sizeof (struct globalmem_dev), GFP_KERNEL); /* 为设备结构图申请内存 */
	if(!globalmem_devp){
		ret = -ENOMEM;
		goto fail_malloc;
	}
	globalmem_setup_cdev(globalmem_devp, 0);
	return 0;

	fail_malloc:
	unregister_chrdev_region(devno, 1); /* 如果失败则注销之前申请的设备号 */
	return ret;
}

module_init(globalmem_init);

static void __exit globalmem_exit(void){
	cdev_del(&globalmem_devp->cdev); /* 将cdev从系统中删除，并释放内存和设备节点 */
	kfree(globalmem_devp);
	unregister_chrdev_region(MKDEV(globalmem_major, 0),1);
}

module_exit(globalmem_exit);

MODULE_AUTHOR("ZHUXC <zhuxc@kernel.org>");
MODULE_LICENSE("GPL v2");

