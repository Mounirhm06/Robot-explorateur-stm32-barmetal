#include "stm32l4xx.h"
#include "Timers.h"


void Tim5_Init (void)
{

		 RCC->APB1ENR1 |= (1<<3);//Timer TIM5 enable
		 TIM5->PSC = 3999;//4Mhz/(3999 +1) => 1KHz (un tick par 1ms)
		 TIM5->ARR = 0xFFFFFFFF;  // Valeur maximale pour le registre 32 bits
		 TIM5->EGR |= TIM_EGR_UG;     // FORCER la prise en compte de PSC (et ARR)
		 TIM5->CR1 |=(1<<0);//enable timer bit CEN/
		 TIM5->SR &= ~(1<<0);//renitialiser le flag UIF*/
		 TIM5->CNT = 0; //renitialiser le compteur


}

// cette fonction renvoie le temps actuel en ms depuis l'allumage de la carte
uint32_t temps(void)
{
	return TIM5->CNT;// lire la valeur du compteur CNT
}


//cette fonction permet de génerer des delay en ms
void delay_ms(uint32_t tms)
{
    uint32_t start = TIM5->CNT;                      // valeur de départ (en ms)
    // Attendre que tms ticks soient passés
    while ((uint32_t)(TIM5->CNT - start) < tms) {
        /* boucle bloquante */
    }
}
