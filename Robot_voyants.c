#include "stm32l4xx.h"
#include "Robot_voyants.h"

// LED verte (PC7) indique une nouvelle reception d'instruction par wifi
void LED_PC7_Init (void)
{
	RCC->AHB2ENR |= (1 << 2);           // Activer l'horloge pour GPIOC
	GPIOC->MODER &= ~(0x3 << (7 * 2));    // Effacer les bits de mode pour PC7
	GPIOC->MODER |= (0x1 << (7 * 2));     // Configurer PC7 en sortie (mode "01")
	GPIOC->OSPEEDR &= ~(0x3 << (7 * 2));  // Effacer les bits de vitesse pour PC7
	GPIOC->OSPEEDR |= (0x0 << (7 * 2));   // Configurer en low speed (mode "00")
	GPIOC->OTYPER &= ~(1 << 7);           // Configurer en push-pull

}

void LED_New_commande_reception(void)
{
	GPIOC->ODR^= (1<<7);//togle la LED PC7
}

//LED Roug(PB3)e indique une d'initialisation du lidar
void LED_PB3_Init(void)
{
    RCC->AHB2ENR |= (1 << 1);           // Activer l'horloge pour GPIOB
    GPIOB->MODER &= ~(0x3 << (3 * 2)); // Effacer les bits de mode pour PB3
    GPIOB->MODER |= (0x1 << (3 * 2));  // Configurer PB3 en sortie (mode "01")
    GPIOB->OSPEEDR &= ~(0x3 << (3 * 2)); // Effacer les bits de vitesse pour PB3
    GPIOB->OSPEEDR |= (0x0 << (3 * 2));  // Configurer en low speed (mode "00")
    GPIOB->OTYPER &= ~(1 << 3);         // Configurer en push-pull
}

void LED_Lidar_erreur(void)
{
    GPIOB->BSRR |= (1 << 3); // Basculer l'état de la LED sur PB3
}

//initilaliser les voyants
void Voyants_Init(void)
{
	LED_PB3_Init();
	LED_PC7_Init();
}
