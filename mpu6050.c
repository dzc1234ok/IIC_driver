#include <linux/module.h>//MODULE_LICENSE
#include <linux/kernel.h>//printk
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/fs.h>  //MKDEV register_chrdev_region  unregister_chrdev_region
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "head.h"


MODULE_LICENSE("GPL");
#define   MPU6050_MARJOR  250
#define   MPU6050_MINOR    0
#define MPU6050_NUM_DEVICE   1

#define SMPRT_DIV  0X19   //0X07
#define CONFIG  0X1A //0X06
#define GYRO_CONFIG  0X1B //0X18
#define ACCEL_CONFIG 0X1C //0X01
#define ACCEL_XOUT_H 0X3B
#define ACCEL_XOUT_L 0X3C
#define ACCEL_YOUT_H 0X3D
#define ACCEL_YOUT_L 0X3E
#define ACCEL_ZOUT_H 0X3F
#define ACCEL_ZOUT_L 0X40
#define TEMP_OUT_H 0X41
#define TEMP_OUT_L 0X42
#define GYRO_XOUT_H 0X43
#define GYRO_XOUT_L 0X44
#define GYRO_YOUT_H 0X45
#define GYRO_YOUT_L 0X46
#define GYRO_ZOUT_H 0X47
#define GYRO_ZOUT_L 0X48
#define PWR_MGMT_1  0X6B //0X00


struct mycdev {

	dev_t devno;
	int ret;
	struct cdev  cdev;

};

struct mycdev  mycdev;
char data[128];
struct i2c_client *mpu6050_client;

struct of_device_id mpu6050_match_table[]  = {

	{.compatible = "invensense,mpu6050"},

};

static struct  i2c_device_id  mpu6050_id_table[] = {
  {"mpu6050",0},

};
/*
 *写一个数据到mpu6050对应的寄存器中
   reg:寄存器的地址
val:写入的数据
 * */
static  void mpu6050_write_byte(unsigned char reg,unsigned char val)
{

	/*有多少个起始位就有多少个消息
	 * 消息的长度以字节表示 
	 * flag : 0  写  1  读*/
	unsigned char txbuf[2] = {reg,val};
	struct i2c_msg msgs[] = {
	
		{mpu6050_client->addr,0,2,txbuf},
		
	
	};
	i2c_transfer(mpu6050_client->adapter,msgs,ARRAY_SIZE(msgs));

}
static unsigned char mpu6050_read_byte(unsigned char reg)
{

	unsigned char txbuf[1] = {reg};
	unsigned  char rxbuf[1]={0};
	struct i2c_msg msgs[] = {
	
		{mpu6050_client->addr,0,1,txbuf},
		
		{mpu6050_client->addr,I2C_M_RD,1,rxbuf}
	
	};
	i2c_transfer(mpu6050_client->adapter,msgs,ARRAY_SIZE(msgs));


	return rxbuf[0];
}
static int mpu6050_open (struct inode *inode, struct file *file){

	printk("mpu6050_open  \n");
	return  0;

}
static ssize_t mpu6050_read (struct file *file, char __user *buf, size_t size, loff_t *loff)
{

	if(size > 128)
		size  = 128;
	if(size <  0)
		return  -ENOMEM;
	if(copy_to_user(buf,data,size))
		return  -EINVAL;
	printk("mpu6050_read  \n");
	return size;
}
static 	int mpu6050_release (struct inode *inode, struct file *file)
{

	printk("mpu6050_release  \n");
	return  0;
}
static long mpu6050_unlocked_ioctl (struct file *file, unsigned int cmd, unsigned long arg)
{

	struct mpu6050_data data;
	switch(cmd)
	{
	
	case MPU6050_GET_GYRO:
		 data.x =mpu6050_read_byte(GYRO_XOUT_L);
	     data.x |=mpu6050_read_byte(GYRO_XOUT_H) <<8;
		 data.y =mpu6050_read_byte(GYRO_YOUT_L);
	     data.y |=mpu6050_read_byte(GYRO_YOUT_H) <<8;
		 data.z =mpu6050_read_byte(GYRO_ZOUT_L);
	     data.z |=mpu6050_read_byte(GYRO_ZOUT_H) <<8;
		break;
	case MPU6050_GET_ACCEL:
		 data.x =mpu6050_read_byte(ACCEL_XOUT_L);
	     data.x |=mpu6050_read_byte(ACCEL_XOUT_H) <<8;
		 data.y =mpu6050_read_byte(ACCEL_YOUT_L);
	     data.y |=mpu6050_read_byte(ACCEL_YOUT_H) <<8;
		 data.z =mpu6050_read_byte(ACCEL_ZOUT_L);
	     data.z |=mpu6050_read_byte(ACCEL_ZOUT_H) <<8;

		break;
	}
	if(copy_to_user((void *)arg,&data,sizeof(data)))
		return  -EINVAL;

	printk("mpu6050_unlocked_ioctl  \n");
	return 0;
}


struct file_operations mpu6050_fops = {


	.owner  =THIS_MODULE,
	.open  = mpu6050_open,
	.read  = mpu6050_read,
	.unlocked_ioctl  = mpu6050_unlocked_ioctl,
	.release = mpu6050_release,

};
static int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *device)
{



	mpu6050_client  = client;
#if  1
	mycdev.devno  = MKDEV(MPU6050_MARJOR,MPU6050_MINOR);
	mycdev.ret  = register_chrdev_region(mycdev.devno,MPU6050_NUM_DEVICE,"mpu6050");
	if(0  != mycdev.ret)
	{
	
		printk("register_chrdev_region fail  \n");
		return  mycdev.ret;
	}
#endif
	cdev_init(&mycdev.cdev,&mpu6050_fops);
	mycdev.ret  = cdev_add(&mycdev.cdev,mycdev.devno,MPU6050_NUM_DEVICE);

	if(0  != mycdev.ret)
	{
	
		printk("cdev_add  fail\n ");
		goto err1;
	}
	mpu6050_write_byte(PWR_MGMT_1,0x00);
	mpu6050_write_byte(SMPRT_DIV,0x07);
	mpu6050_write_byte(CONFIG,0x06);
	mpu6050_write_byte(GYRO_CONFIG,0x18);
	mpu6050_write_byte(ACCEL_CONFIG,0x01);

	printk("match  ok  \n");
	return  0;
err1:
	unregister_chrdev_region(mycdev.devno,MPU6050_NUM_DEVICE);
	return  mycdev.ret;
}
static int mpu6050_remove(struct i2c_client *client)
{

	cdev_del(&mycdev.cdev);
	unregister_chrdev_region(mycdev.devno,MPU6050_NUM_DEVICE);
	printk("mpu6050_remove  \n");
	return  0;
}
static struct i2c_driver mpu6050_driver = {

  .driver = {
  
	  .name  = "mpu6050",
	  .of_match_table = mpu6050_match_table,//使用设备树匹配 i2c_client
  },
  .id_table = mpu6050_id_table,//有平台代码匹配i2c_client
  .probe  = mpu6050_probe,
  .remove  = mpu6050_remove,


};
int mpu6050_init(void)
{

	i2c_add_driver(&mpu6050_driver);
	printk("mpu6050_init  \n");
	return  0;
}
void mpu6050_exit(void)
{

	i2c_del_driver(&mpu6050_driver);
	printk("mpu6050_exit  \n");
}
module_init(mpu6050_init);
module_exit(mpu6050_exit);
