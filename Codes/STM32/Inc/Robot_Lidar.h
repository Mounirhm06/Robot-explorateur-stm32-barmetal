#ifndef LIDAR_H
#define LIDAR_H

	// Commandes pour contrôler le RPLIDAR
	#define RPLIDAR_CMD_START_SCAN        0x20
	#define RPLIDAR_CMD_STOP              0x25
	#define RPLIDAR_CMD_RESET 			  0x40
	#define RPLIDAR_CMD_SYNC_BYTE      	  0xA5
	#define RPLIDAR_CMD_GET_HEALTH		  0x52
	#define RPLIDAR_CMD_GET_SAMPLERATE    0x59
	#define RPLIDAR_CMD_SPEED_CTRL1 	  0xA8
	#define RPLIDAR_CMD_SPEED_CTRL2 	  0x02
	#define RPLIDAR_RESP_MEASUREMENT_SYNCBIT        (0x1<<0)

	//mesures
	uint8_t Rplidar_health_statut(uint16_t *error_msg);
	void Rplidar_Reset(void);
	uint8_t Rplidar_Init(void);
	uint8_t Rplidar_Start_Scan(uint8_t *data);
	void Rplidar_Stop_Scan(void);
	void Rplidar_mesures(uint8_t *pPoints ,uint16_t nbPoints );
	void Rplidar_map(uint8_t *ppoint, float *Angle, float *Distance, uint16_t nbpoints);

	//moteur
	void Tim3_Init (void);
	void PWM_TIM3_CH1_config (void);
	void Rplidar_PWM_Pin_Init(void);
	void RPLIDAR_SetMotorPWM(uint16_t speedL);
	void Lidar_PWM_Init(void);

#endif
