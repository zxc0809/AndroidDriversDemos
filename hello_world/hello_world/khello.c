#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("innoink");
MODULE_DESCRIPTION("hello world module");

static int hello_data __initdata = 1;  //only use in init project...

static char *user_name = "i am zxc...";
module_param(user_name,charp,S_IRUGO);

static int age = 26;
module_param(age,int,S_IRUGO);

static int __init khello_init(void)
{
	printk(KERN_INFO "hello world! %d\n",hello_data);
	printk(KERN_INFO "USER NAME IS %s\n",user_name);
	printk(KERN_INFO "USER AGE IS %d\n", age);
	return 0;
}

static void __exit khello_exit(void)
{
	printk(KERN_INFO "hello exit!\n");
}

module_init(khello_init);
module_exit(khello_exit);

