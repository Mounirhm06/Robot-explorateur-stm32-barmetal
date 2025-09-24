#ifndef ROBOT_CONTROL_DIRECTION_H
#define ROBOT_CONTROL_DIRECTION_H
	typedef enum{FORWARD,BACKWARD,STOP}robot_direction;
	typedef enum{RIGHT,LEFT}rotation_direction;
	void moteur_M1_M3_initialisation(void);
	void moteur_M2_M4_initialisation(void);
	void moteur_M1M3_Forward(void);
	void moteur_M1M3_Backward(void);
	void moteur_M2M4_Forward(void);
	void moteur_M2M4_Backward(void);
	void Robot_Direction_control(robot_direction direction, uint8_t robot_speed);
	void moteur_M1M3_Speed(int speed);
	void moteur_M2M4_Speed(int speed);
	void robot_rotation_control(rotation_direction rotation, uint8_t speed);
	void TIM2_init(void);
	void PWM_TIM2_CH1_config(void);
	void PWM_TIM2_CH2_config (void);
	void Robot_PWM_Init(void);
	void Moteurs_Init (void);
	void Angle_transformation(int16_t *pAngle,uint8_t nbPoints);

#endif
