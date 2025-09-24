#include <stdio.h>
#include <stdlib.h>
#include"stm32l4xx.h"
#include "main.h"
#include "Robot_encodeurs.h"
//#include "Timers.h"


uint32_t distanceM1 = 0;//distance parcourue par le moteur M1
uint32_t distanceM2 = 0;//distance parcourue par le moteur M2
uint8_t flag_new_distanceM1 = 0;
uint8_t flag_new_distanceM2 = 0;

void encodeur1_Pin_Init(void)
{
	/*encodeurs M1, Pin PC0*/
	RCC->AHB2ENR |= (1<<2); // enable GPIOC
	GPIOC->MODER &= ~(0x3<<0); // mode input
	GPIOC->PUPDR &= ~(0x3<<0);//effacer les bits
	GPIOC->PUPDR |= (1<<0);// pull up

}

void encodeur2_Pin_Init(void)
{
	/*encodeurs M2, Pin PC1*/
	RCC->AHB2ENR |= (1<<2); // enable GPIOC
	GPIOC->MODER &= ~(0x3<<2); // mode input
	GPIOC->PUPDR &= ~(0x3<<2);//effacer les bits
	GPIOC->PUPDR |= (1<<2);//pull up

}

void encodeur1_interupt_Init(void){
	/*PIN PC0 , encodeur 1 moteur M1*/
	RCC->APB2ENR |= (1<<0); // enable SYSCFGEN
	SYSCFG->EXTICR[0]&= ~(0xF<<0); // effacer les bits EXTI0
	SYSCFG->EXTICR[0]|= (2<<0);//connecter la broche PC0 a EXTI0
	EXTI->IMR1 |= (1<<0); // demasquer la ligne EXTI0
	//->RTSR1 |= (1<<0); //detection du front montant
	EXTI->FTSR1 |= (1<<0); // detection du front descendant
	NVIC_EnableIRQ(EXTI0_IRQn);//activr l'interruption de EXTI0 au niveau de NVIC
	NVIC_SetPriority(EXTI0_IRQn, 2);//defenir une priorite 2 pour l'ISR
}

void encodeur2_interupt_Init(void){
	/*PIN PC1 , encodeur 2 moteur M2*/
	RCC->APB2ENR |= (1<<0); // enable SYSCFGEN
	SYSCFG->EXTICR[0]&= ~(0xF<<4); // effacer les bits EXTI1
	SYSCFG->EXTICR[0]|= (2<<4);//connecter la broche PC1 a EXTI1
	EXTI->IMR1 |= (1<<1); // demasquer la ligne EXTI1
	//EXTI->RTSR1 |= (1<<1); //detection du front montant
	EXTI->FTSR1 |= (1<<1); // detection du front descendant
	NVIC_EnableIRQ(EXTI1_IRQn);//activr l'interruption de EXTI0 au niveau de NVIC
	NVIC_SetPriority(EXTI1_IRQn, 3);//defenir une priorite 3 pour l'ISR
}


void EXTI0_IRQHandler (void)
{
	//ISR sur PC0
	if((EXTI->PR1) & (1<<0)){
		//verifier si l'IRQ vient de EXTI0
		HAL_Delay(3);//delay 2ms pour la stabilite du capteur
		distanceM1++;
		flag_new_distanceM1 = 1;
		EXTI->PR1 |= (1<<0); // effacer le flag PIF0  pour la prochaine interruption
	}
}

void EXTI1_IRQHandler (void)
{
	//ISR sur PC1
	if((EXTI->PR1) & (1<<1)){
		//verifier si l'IRQ vient de EXTI0
		HAL_Delay(3);//delay 2ms pour la stabilite du capteur
		distanceM2++;
		flag_new_distanceM2 = 1;
		EXTI->PR1 |= (1<<1); // effacer le flag PIF0  pour la prochaine interruption
	}
}

uint32_t distance_parcou_M1 (void)
{
	return distanceM1;
}

uint32_t distance_parcou_M2 (void)
{
	return distanceM2;
}

void renitialiser_distancd_M1 (void)
{
	distanceM1 = 0;
}

void renitialiser_distancd_M2 (void)
{
	distanceM2 = 0;
}


uint8_t new_distanceM1(void)
{
	if(flag_new_distanceM1)
	{
		flag_new_distanceM1 = 0;//effacer le flag
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t new_distanceM2(void)
{
	if(flag_new_distanceM2)
	{
		flag_new_distanceM2 = 0;//effacer le flag
		return 1;
	}
	else {
		return 0;
	}
}

void Robot_encodeurs_Init(void)
{
	encodeur1_Pin_Init();
	encodeur2_Pin_Init();
	encodeur1_interupt_Init();
	encodeur2_interupt_Init();
}
