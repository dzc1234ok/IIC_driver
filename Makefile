#KERNELDIR  :=/lib/modules/$(shell  uname  -r)/build
KERNELDIR  :=/home/linux/Exynos4412/linux-3.14-fs4412


test:
	make  -C $(KERNELDIR)  M=$(shell  pwd) modules
#	sudo  cp  *.ko   /source/fs4412_rootfs 

run:
	arm-none-linux-gnueabi-gcc  test.c  -o run
	sudo  cp  run  /source/fs4412_rootfs 
clean:
	rm  -rf  *.ko  *.o  *.mod.*   modules*  Module*
	
	
obj-m = mpu6050.o
