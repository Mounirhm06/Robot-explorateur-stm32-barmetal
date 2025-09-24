#include<stdio.h>
#include<stdlib.h>
#include "stm32l4xx.h"
#include "Robot_control_direction.h"
#include "Robot_encodeurs.h"

const float impul_to_degre = 7.5;// permet de transformer les impulsions des encodeurs en angles

/*****************		control du sens des moteurs 	*******************************/

void moteur_M1_M3_initialisation(void)
{
	/*MOTEUR AVANT DROIT + MOTEUR ARRIERE DROIT */

    /*  PIN PA0 controler le sens de rotation (PIN D4 du moteur)*/
    RCC->AHB2ENR |= (1 << 0); // enable GPIOA /
    GPIOA->MODER |= (1 << 0); // configurer PA0 en sortie /
    GPIOA->MODER &= ~(1 << 1);
    GPIOA->OTYPER &= ~(1 << 0); // Push-Pull configuration //
    GPIOA->OSPEEDR &= ~(0x3 << 0); // Low Speed

    /*PIN PA1 controler la vitesse de rotation(PWM)(PIN D5 du moteur) */
    GPIOA->MODER &= ~(0x3 << 2);//effacer les bits MODE1
    GPIOA->MODER |= (0x2 << 2);//mode alternatif
    GPIOA->OTYPER &= ~(1 << 1);//push pull
    GPIOA->OSPEEDR &= ~(0x3 << 2);//effacer les bits OSPEED2
    GPIOA->OSPEEDR |= (0x2<<2);//high speed
    GPIOA->AFR[0] &= ~(0xF<<4);//effacer les bits AFSEL1
    GPIOA->AFR[0] |= (1<<4);//fonction AF1(TIM2_CH2)

}

void moteur_M2_M4_initialisation(void)
{
	/*MOTEUR AVANT GAUCHE + MOTEUR ARRIERE GAUCHE */

	/* PIN PA4 pour le sens de rotation(PIN D7 du moteur)*/
	GPIOA->MODER |= (1 << 8); // configurer PA4 en sortie
	GPIOA->MODER &= ~(1 << 9);
	GPIOA->OTYPER &= ~(1 << 4); // Push-Pull configuration
	GPIOA->OSPEEDR &= ~(0x3 << 8); // Low Speed

	/*PIN PA5 pour la vitesse de rotation(PWM),(PIN D6 du moteur)*/
	GPIOA->MODER &= ~(0x3 << 10); // effacer les bits MODER
	GPIOA->MODER |= (0x2 << 10);//mode alternatif
	GPIOA->OTYPER &= ~(1 << 5); // Push-Pull configuration
	GPIOA->OSPEEDR &= ~(0x3 << 10); // effacer les bits OSPEEDER
	GPIOA->OSPEEDR |= (0x2<<10); //high speed
	GPIOA->AFR[0] &= ~(0xF<<20);//effacer les bits AFSEL5
	GPIOA->AFR[0] |= (1<<20);//fonction AF1(TIM2_CH1)
}


/*reglage du sens du retation des moteurs*/
void moteur_M1M3_Forward(void)
{
	GPIOA->BSRR |= (1<<0);//set PA0
}

void moteur_M1M3_Backward(void)
{
	GPIOA->BSRR |= (1<<16);//reset PA0
}

void moteur_M2M4_Forward(void)
{
	GPIOA->BSRR |= (1 << 4);  // set PA4
}

void moteur_M2M4_Backward(void)
{
	 GPIOA->BSRR |= (1 << 20); // reset PA4
}

/*reglage des vitesse par signal PWM*/
void moteur_M1M3_Speed(int speed)
{
	TIM2->CCR2 = speed; // rapport cyclique de speed/100

}
void moteur_M2M4_Speed(int speed)
{
	// CCR1 = 0 => rapport cyclique 0, PWM = 0V , CCR1 = 100 , rapport cyclique 1 , PWM = 3.3V
	TIM2->CCR1 = speed; // rapport cyclique de speed/100
}

/*Pour avancer,reculer,arreter*/
void Robot_Direction_control(robot_direction direction, uint8_t robot_speed)
{
	switch (direction){

	case FORWARD  : moteur_M2M4_Forward();
					moteur_M1M3_Forward();
					moteur_M1M3_Speed(robot_speed);
					moteur_M2M4_Speed(robot_speed);
				    break;

	case BACKWARD : moteur_M1M3_Backward();
					moteur_M2M4_Backward();
					moteur_M1M3_Speed(robot_speed);
					moteur_M2M4_Speed(robot_speed);
					break;

	case STOP     :moteur_M1M3_Speed(0);
				   moteur_M2M4_Speed(0);

				   break;
	}
}


/*pour faire tourner le robot gauche droite*/
void robot_rotation_control(rotation_direction rotation,uint8_t speed)
{
	switch (rotation){
	case RIGHT :moteur_M2M4_Forward();
				moteur_M1M3_Forward();
				moteur_M2M4_Speed(speed);
				moteur_M1M3_Speed(0);//le moteur M1M3 tourne par default lentement par rapport à M2M4
				break;
	case LEFT : moteur_M1M3_Forward();
				moteur_M2M4_Forward();
				moteur_M1M3_Speed(speed);
				moteur_M2M4_Speed(0);
				break;
	}
}

// les angles envoyees du pc est 0<angle<360 , or le capteur mesure -180<angle<180 donc on transforme les angles par cette fonction
void Angle_transformation(int16_t *pAngle,uint8_t nbPoints)
{
	uint8_t i;
	for(i = 0; i<nbPoints;i++){
		if(pAngle[i]<= 180){
			//si l'angle est positif on la laisse telle qu'elle est [0;180]
			pAngle[i] = pAngle[i];
		}
		else {
			// si l'angle >180 , on la transforme en negatif [-180:0]
			pAngle[i] = pAngle[i] - 360;

		}
	}
}

/*Initialiser tous les moteurs d'un coup*/
void Moteurs_Init (void)
{
	moteur_M1_M3_initialisation();
	moteur_M2_M4_initialisation();
}




/*********************************** PWM control (vitesse des moteurs) ********************************************/
void TIM2_init(void)
{
	RCC->APB1ENR1 |= (1<<0); // enable tim2
	TIM2->PSC = 399 ; //4MHZ/400 = 10KHz (1 tick par 0.1ms)
	TIM2->ARR = 99; // overflow a 10ms=> frequence PWM 100Hz
	TIM2->CR1 |= (1<<0); //activer le compteur CNT(bit CEN)
	TIM2->SR &= ~(1<<0); // effacer le flag UIF
}

void PWM_TIM2_CH1_config (void)
{
	// utiliser le chanel 1  du TIM2 (moteurs M2_M4), pin PA5//
	TIM2->CCR1 = 00; // rapport cyclique de 00/100
	TIM2->CCMR1 &= ~(0x7<<4); // effacer les bits OC1M
	TIM2->CCMR1 |= (0x6<<4); // PWM (si CNT<=CCR => sortie up si CNT=>CRR => sortie down)
	TIM2->CCMR1 |= (1<<3); // activation d'autoreload à chaque update event(bit OC1PE)
	TIM2->CCER &= ~(1<<1); // polarity 0 (bit CC1P)
	TIM2->CCER |= (1<<0); // activer le mode compare sur le canal1(bit CC1E)
}

void PWM_TIM2_CH2_config(void)
{
	// utiliser le chanel 2  du TIM2 (moteurs M1_M3), pin PA1//
	TIM2->CCR2 = 00;// rapport cycqlique de 00/100
	TIM2->CCMR1 &= ~(0x7<<12);//effacer les bits OC2M
	TIM2->CCMR1 |= (0x6<<12);//PWM
	TIM2->CCMR1 |= (1<<11); // activation d'autoreload bit OC2PE
	TIM2->CCER  &= ~(1<<5); // polarity 0 (bit CC2P)
	TIM2->CCER |= (1<<4); //activer le mode compare sur le canal2(bit CC2E)

}

/*Initialiser les PWM d'un coup*/
void Robot_PWM_Init(void)
{
	 TIM2_init();
	 PWM_TIM2_CH1_config();
	 PWM_TIM2_CH2_config ();
}






