#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#define BUFF_SIZE 120
#define MAX_STR 100+1

MODULE_LICENSE("Dual BSD/GPL");

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

//DECLARE_WAIT_QUEUE_HEAD(readQ);
DECLARE_WAIT_QUEUE_HEAD(writeQ);
struct semaphore sem;

char stred[MAX_STR];
int pos = 0;
int endRead = 0;

int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stred_open,
	.read = stred_read,
	.write = stred_write,
	.release = stred_close,
};


int stred_open(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Uspesno otvoren fajl\n");
		return 0;
}

int stred_close(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Uspesno zatvoren fajl\n");
		printk(KERN_INFO "%s\n",stred);
		return 0;
}

ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	int ret;
	char buff[BUFF_SIZE];
	long int len;
	if (endRead){
		endRead = 0;
		pos = 0;
		printk(KERN_INFO "Uspesno procitano iz fajla\n");
		up(&sem);
		return 0;
	}
	if (pos == 0)
		if (down_interruptible(&sem))
		return -ERESTARTSYS;
	len = scnprintf(buff,BUFF_SIZE , "%c", stred[pos]);
	ret = copy_to_user(buffer, buff, len);
	if(ret)
		return -EFAULT;
	pos ++;
	if (pos == strlen(stred)+1) 
	{
		endRead = 1;
		ret = copy_to_user(buffer,"\n",1);
		if (ret)
			return -EFAULT;
	}
	return len;
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
	char buff[BUFF_SIZE], str[MAX_STR];
	int ret,i,broj;

	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	buff[length-1] = '\0';

	//1. Upis stringa
	ret = 0;
	if (buff[0]=='s'&&buff[1]=='t'&&buff[2]=='r'&&buff[3]=='i'&&buff[4]=='n'&&buff[5]=='g'&&buff[6]=='=')
		ret = 1;
	if(ret==1)
	{
			if (length>MAX_STR+7)
			{
				printk(KERN_INFO "Uneti string predugacak!");
				return -1;
			}
			if (down_interruptible(&sem))
				return -ERESTARTSYS;
			for(i=0;i<length-7;i++) //treba da u str upisem ono sto se obradjuje
			{
				str[0+i]=buff[7+i];
			}
			memset(stred,0,MAX_STR); //overwrite
			strcpy(stred,str); //upis
			printk(KERN_INFO "String \"%s\" uspesno upisan\n", str);
		//	printk(KERN_INFO "%s",stred );
			up(&sem);
			wake_up_interruptible(&writeQ);
			return length; 
	}


	//2. Brisanje stringa
	ret = 1;
	ret = strcmp(buff,"clear");
	if (ret==0)
	{
		if (down_interruptible(&sem))
			return -ERESTARTSYS;
		memset(stred,0,MAX_STR);
		printk(KERN_INFO "String uspesno obrisan");
		up(&sem);
		wake_up_interruptible(&writeQ);
		return length;
	}

	//3. Brisanje space-ova iz stringa
	ret = 1;
	ret = strcmp(buff,"shrink");
	if (ret == 0)
	{
		if (down_interruptible(&sem))
			return -ERESTARTSYS;
		strcpy(stred,strim(stred));
		printk(KERN_INFO "Operacija \"shrink\" uspesno izvrsena");
		up(&sem);
		wake_up_interruptible(&writeQ);
		return length;
	}

	//4. Dodavanje stringa na postojeci
	ret = 0;
	if (buff[0]=='a'&&buff[1]=='p'&&buff[2]=='p'&&buff[3]=='e'&&buff[4]=='n'&&buff[5]=='d'&&buff[6]=='=')
		ret = 1;
	if (ret == 1)
	{
		if (down_interruptible(&sem))
			return -ERESTARTSYS;
		while (strlen(stred)+ length-7 > MAX_STR)
		{
			up(&sem);
			if (wait_event_interruptible(writeQ,(strlen(stred)+length-7<=MAX_STR)))
				return -ERESTARTSYS;
			if (down_interruptible(&sem))
				return -ERESTARTSYS;
		}
		for (i=0;i<length-7;i++) 
			str[0+i]= buff[7+i];
		strcpy(stred,strcat(stred,str));
		printk(KERN_INFO "Operacija \"append\" uspesno izvrsena");
		up(&sem);
		return length;
	}

	//5. Brisanje broja karaktera sa kraja bafera
	ret = 0;
	ret = sscanf(buff,"truncate=%d",&broj); //broj karaktera za brisanje
	if (ret == 1)
	{
		if (broj>strlen(stred))
		{
			printk(KERN_INFO "Bafer nije toliko velik");
			return -1;
		}
		if (down_interruptible(&sem))
			return -ERESTARTSYS;
		memzero_explicit(&stred[strlen(stred)-broj],broj);
		printk(KERN_INFO "Operacija \"truncate\" uspesno izvrsena");
		up(&sem);
		wake_up_interruptible(&writeQ);
		return length;
	}

	//6. Brisanje pojave niza kar. iz bafera

	ret = 0;
	if (buff[0]=='r'&&buff[1]=='e'&&buff[2]=='m'&&buff[3]=='o'&&buff[4]=='v'&&buff[5]=='e'&&buff[6]=='=')
		ret = 1;
	if (ret == 1)
	{
		size_t len;
		if (length-7>strlen(stred))
		{
			printk(KERN_INFO "Izraz za brisanje veci je od bafera");
			return -1;
		}
		if (down_interruptible(&sem))
			return -ERESTARTSYS;
		for(i=0;i<length-7;i++)
			str[i]=buff[i+7]; //u str je ono za izbacivanje
		if (strstr(stred,str)==NULL)
		{
			printk(KERN_INFO "Niz karaktera \"%s\" ne nalazi se u baferu",str);
			up(&sem);
			return -1;
		}
		len = strlen(str);
		if (len > 0)
		{
			char* p = stred;
			while((p = strstr(p,str)) != NULL)
				memmove(p,p + len,strlen(p+len) + 1);
		}
		printk(KERN_INFO "Operacija \"remove\" uspesno izvrsena");
		up(&sem);
		wake_up_interruptible(&writeQ);
		return length;
	}

	// Nepravilan unos

	printk(KERN_INFO "Nepostojuca komanda.");
	return -1;
}

static int __init stred_init(void)
{
   int ret = 0;
	int i=0;

	sema_init(&sem,1);
	//Inicijalizacija niza
	for (i=0; i<MAX_STR; i++)
		stred[i] = 0;

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stred");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "stred_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   my_device = device_create(my_class, NULL, my_dev_id, NULL, "stred");
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Zdravo svete!\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit stred_exit(void)
{
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Zbogom surovi svete\n");
}


module_init(stred_init);
module_exit(stred_exit);
