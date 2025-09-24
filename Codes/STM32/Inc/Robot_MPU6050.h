#ifndef ROBOT_MPU6050_H
#define ROBOT_MPU6050_H
	void MPU6050_Init(void);
	void Gyro_mesures(float *pGyro_mesur);
	float yaw_value(float *pGyro_mesures,uint32_t tempsTi,uint32_t tempsTiPlus);
	void Reset_yawVal(void);

#endif
