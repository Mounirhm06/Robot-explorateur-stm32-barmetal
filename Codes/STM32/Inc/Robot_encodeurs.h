#ifndef ROBOT_ENCODEURS_H
#define ROBOT_ENCODEURS_H
#include <stdio.h>
	void encodeur1_Pin_Init(void);
	void encodeur2_Pin_Init(void);
	void encodeur1_interupt_Init(void);
	void encodeur2_interupt_Init(void);
	uint32_t distance_parcou_M1 (void);
	uint32_t distance_parcou_M2 (void);
	void Robot_encodeurs_Init(void);
	void renitialiser_distancd_M1 (void);
	void renitialiser_distancd_M2 (void);
	uint8_t new_distanceM1(void);
	uint8_t new_distanceM2(void);

#endif
