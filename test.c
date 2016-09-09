#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include "head.h"

int main(int argc, const char *argv[])
{
	int fd;

	struct mpu6050_data data;
	if(argc  <  2)
	{
	
		printf("usage   %s  argv[1] \n",argv[0]);
		exit(EXIT_FAILURE);
	}

	fd  =  open(argv[1],O_RDWR);
	if(fd  <  0)
	{
	
		perror("open fail ");
		exit(EXIT_FAILURE);
	}
	printf("open success  \n");



	while(1)
	{
	
		printf("read data :\n");
		ioctl(fd,MPU6050_GET_GYRO,&data);
		printf("gyro data :x =%x y = %x z = %x\n",
				data.x,data.y,data.z);
		ioctl(fd,MPU6050_GET_ACCEL,&data);
		printf("ACCEL data :x =%x y = %x z = %x\n",
				data.x,data.y,data.z);

		sleep(1);

	}
	close(fd);
	return 0;
}

