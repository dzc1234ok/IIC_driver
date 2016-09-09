#ifndef __HEAD_H__
#define __HEAD_H__
struct mpu6050_data {

	short x;
	short y;
	short z;

};
#define MPU6050_GET_GYRO  _IOR('K',0,struct mpu6050_data)
#define MPU6050_GET_ACCEL  _IOR('K',1,struct mpu6050_data)

#endif
