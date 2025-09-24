#include <stdio.h>
#include "stm32l4xx.h"
#include "Robot_Communication.h"
#include "Timers.h"


uint8_t buffer_USART3 [102] = {0};//max 20points recu , 2char*20 distance + 3char*20 angle + 1char nbpoints + 1char \n
uint8_t flag_fin_reception_USART3 = 0;
uint8_t nb_Instructions_recieved = 0;//stocker dans buffer_reception[0]

uint8_t buffer_USART1 [5] = {0};
uint8_t flag_fin_reception_USART1 = 0;
int nbChar_lidar_reception = 0;


/************************** USART3 communication wifi(ESP8266) ***********************/
void USART3_init(void)
{
	/*initialiser le pin PC10 pour TX*/
		RCC->AHB2ENR |= (1<<2); //enable GPIOC
		GPIOC->MODER &= ~(0x3<<20);//effcer les bits mode10
		GPIOC->MODER |=  (0x2<<20);// choisir le mode alternatif pour la broche PC10(c'est le mode utiliser pour l'USART)
		GPIOC->PUPDR &= ~(0x3<<20);//effacer les bits PUPD10
		//GPIOC->PUPDR |= (1<<20);// chooisir Pull up (pour garentire que la ligne est a 1 par default(voir protocol USART)
		GPIOC->OSPEEDR &= ~(0x3<<20);//effacer les bits OSPEED10
		GPIOC->OSPEEDR |= (0x2<<20);//high speed
		GPIOC->AFR[1] &= ~(0xF<<8);//effacer les bits 8,9,10,11
		GPIOC->AFR[1] |=(0x7<<8);//fonction AF7 (USART3_TX)

		/*initialiser le pin PC11 pour RX*/
		GPIOC->MODER &= ~(0x3<<22);//effcer les bits mode11
		GPIOC->MODER |=  (0x2<<22);// choisir le mode alternatif pour la broche PC11(c'est le mode utiliser pour l'USART)
		GPIOC->PUPDR &= ~(0x3<<22);//effacer les bits PUPD11
		//GPIOC->PUPDR |= (1<<22);// chooisir Pull up (pour garentire que la ligne est a 1 par default(voir protocol USART)
		GPIOC->OSPEEDR &= ~(0x3<<22);//effacer les bits OSPEED11
		GPIOC->OSPEEDR |= (0x2<<22);//high speed
		GPIOC->AFR[1] &= ~(0xF<<12);//effacer les bits 12,13,14,15
		GPIOC->AFR[1] |=(0x7<<12);//fonction AF7 (USART3_RX)

		/*initialiser USART3*/
		RCC->APB1ENR1|= (1<<18); // enable USART3 clock
		RCC->CCIPR |= (1UL << 4); // Sélectionner SYSCLK comme source d’horloge pour USART3
		USART3->CR1 &=~(1<<0);//desactiver l'USART avant le reglage (UE = 0)
		//choisir la taille 8bits pour le message envoye
		USART3->CR1 &=~(1<<28);// M1 = 0
		USART3->CR1 &=~(1<<12);// M0 = 0
		USART3->BRR = 417;//frequence d'horlohe 4Mhz, 9600 bauds
		//USART3->BRR = 35;//frequence d'horlohe 4Mhz, 115200 bauds
		USART3->CR2 &= ~(0x3<<12); // 1bit de STOP apres les donnees
		USART3->CR1 |= (1<<0);//reactiver l'USART(UE = 1)
		//Transmission//
		USART3->CR1 |= (1<<3);  // activer la transmission TX (bit TE)
		while ((USART3->ISR & (1 << 21)) == 0) {}; // Attendre TEACK=1, indique que l'usart est pret pour la transmission
		//Reception//
		USART3->CR1 |= (1<<2); // activer la reception RX(bit RE)
		while ((USART3->ISR & (1 << 22)) == 0) {};// attendre REACK = 1 , indique que l'usart est pret pour la reception
		USART3->CR1 |= (1<<5);//activer les interruption pour la reception, bit RXNEIE
		NVIC_EnableIRQ(USART3_IRQn);//activer l'interruption au niveau de NVIC
		NVIC_SetPriority(USART3_IRQn,1);//interruption de niveau 1

}

void USART3_send_char(uint8_t message)
{
	while (!(USART3->ISR & (1<<7))){}; // Attendre que le registre TDR devient libre(TXE=1), ce flag est effacer par la lecture de registre TDR
	USART3->TDR = message;
	//while (!(USART3->ISR & (1<<6))){};  // Attendre fin transmission (TC=1), indique la fin totale de transmission
}

// remplir le tableau envoye depuis le main, contient les instructions des distances a parcourir et l'angle pour tourner
void Usart3_recieved_Instructions(uint8_t * pDistance, int16_t * pAngle )
{

	uint8_t i,j=0;
	for(i = 2; i<2*nb_Instructions_recieved +1 ; i+=2){
		//chaque distance est codee sur 2 octets
		pDistance[j] = (buffer_USART3[i]-48)*10 + (buffer_USART3[i+1]-48);
		j++;
	}
	j = 0;
	for(i = 2*nb_Instructions_recieved +2; i<5*nb_Instructions_recieved +2 ; i+=3){
		//chauque angle est codee sur 3 octets
		pAngle[j] = (buffer_USART3[i]-48)*100 + (buffer_USART3[i+1]-48)*10 + (buffer_USART3[i+2]-48);
		j++;
		}
}


/*ISR lancee une fois RXNE est a 1*/
void USART3_IRQHandler (void)
{
	uint8_t char_recieved = 0;
	static uint8_t i;//nombre ce caracteres recu
	char_recieved = USART3->RDR;//lire la valeur de l'octet recu
	if(char_recieved == '\n'){
		//GPIOA->ODR ^= (1<<1);//toggle LED PA1
		flag_fin_reception_USART3 = 1;//indiquer la fin de reception de la totalite du message
		nb_Instructions_recieved = (buffer_USART3[0]-48)*10 + buffer_USART3[1]-48; // nombre de points recu du PC(2 char pour distance et 3 char pour angle)
		i = 0;//remettre le compteur a zero
	 }
	else {
		if(char_recieved != '\r'){//supprimer le retour chariot
			buffer_USART3[i] = char_recieved;
			i++;
		}
	}

}

uint8_t Fin_reception_Usart3 (void)
{
	if(flag_fin_reception_USART3 ){
		flag_fin_reception_USART3 = 0; // effacer le flag
		return 1;
	}
	else{
		return 0;
	}

}

uint8_t nbInstructions_recu (void)
{
	return nb_Instructions_recieved;
}

void reset_buffer_USART3(void)
{
	uint8_t i;
	for(i = 0 ;i<101;i++){
		buffer_USART3[i] = 0;
	}

}



/* ******************** USART1 communication LIDAR  **************************************/
void USART1_init(void)
{
	/*initialiser le pin PA9 pour TX*/
	RCC->AHB2ENR |= (1<<0); //enable GPIOA
	GPIOA->MODER &= ~(0x3<<18);//effacer les bits mode9
	GPIOA->MODER |=  (0x2<<18);// choisir le mode alternatif pour la broche PA9
	GPIOA->PUPDR &= ~(0x3<<18);//effacer les bits PUPD9
	GPIOA->OSPEEDR &= ~(0x3<<18);//effacer les bits OSPEED9
	GPIOA->OSPEEDR |= (0x2<<18);//high speed
	GPIOA->AFR[1] &= ~(0xF<<4);//effacer les bits 4,5,6,7
	GPIOA->AFR[1] |=(0x7<<4);//fonction AF7 (USART1_TX)

	/*initialiser le pin PA10 pour RX*/
	GPIOA->MODER &= ~(0x3<<20);//effacer les bits mode10
	GPIOA->MODER |=  (0x2<<20);// choisir le mode alternatif pour la broche PA10
	GPIOA->PUPDR &= ~(0x3<<20);//effacer les bits PUPD10
	GPIOA->OSPEEDR &= ~(0x3<<20);//effacer les bits OSPEED10
	GPIOA->OSPEEDR |= (0x2<<20);//high speed
	GPIOA->AFR[1] &= ~(0xF<<8);//effacer les bits 8,9,10,11
	GPIOA->AFR[1] |=(0x7<<8);//fonction AF7 (USART1_RX)

	/*initialiser USART1*/
	RCC->APB2ENR |= (1<<14); // enable USART1 clock
	RCC->CCIPR |= (1UL << 2); // Sélectionner SYSCLK comme source d’horloge pour USART1
	USART1->CR1 &=~(1<<0);//désactiver l'USART avant le réglage (UE = 0)
	USART1->CR1 &=~(1<<28);// M1 = 0
	USART1->CR1 &=~(1<<12);// M0 = 0
	//USART1->BRR = 417;//fréquence d'horloge 4Mhz, 9600 bauds
	USART1->BRR = 35;//fréquence d'horloge 4Mhz, 115200 bauds
	USART1->CR2 &= ~(0x3<<12); // 1bit de STOP après les données
	USART1->CR1 |= (1<<0);//réactiver l'USART (UE = 1)
	//Transmission//
	USART1->CR1 |= (1<<3);  // activer la transmission TX (bit TE)
	while ((USART1->ISR & (1 << 21)) == 0) {}; // Attendre TEACK=1
	//Réception//
	USART1->CR1 |= (1<<2); // activer la réception RX (bit RE)
	while ((USART1->ISR & (1 << 22)) == 0) {};// attendre REACK = 1
	//USART1->CR1 |= (1<<5);//activer les interruptions pour la réception (bit RXNEIE)
	//NVIC_EnableIRQ(USART1_IRQn);//activer l'interruption au niveau de NVIC
	//NVIC_SetPriority(USART1_IRQn,1);//interruption de niveau 1
}

//activer l'interruption
void USART1_Interruption_Inable(void)
{
	USART1->CR1 |= (1<<5);//activer les interruptions pour la réception (bit RXNEIE)
	NVIC_EnableIRQ(USART1_IRQn);//activer l'interruption au niveau de NVIC
	NVIC_SetPriority(USART1_IRQn,2);//interruption de niveau 2/
}

//disactiver l'interruption
void USART1_Interruption_Disable(void)
{
	while (USART1->ISR & (1 << 5)) {
	        volatile uint8_t dummy = USART1->RDR; // Lire et ignorer les données résiduelles
	    }
	USART1->CR1 &= ~(1<<5);//disactiver l'interruption (bit RXNEIE)
}

// envoyer un octet par usart/
void USART1_send_char(uint8_t message)
{
	while (!(USART1->ISR & (1<<7))){
		// Attendre que le registre TDR devienne libre (TXE=1)
	};
	USART1->TDR = message;
}

//recevoir n octet par usart1
void USART1_char_recieved(uint8_t *pdata , uint8_t nbdata)
{
	uint32_t timeout = temps();//lire la valeur du temps a l'appel de la fonction en ms
	uint8_t i = 0;
	for(i = 0;i<nbdata;i++){
		while(!(USART1->ISR & (1<<5))){
			if((temps() - timeout)>1000){
				//si le lidar ne reponds pas au bout de 1s on quitte la boucle
				GPIOC->BSRR |= (1<<7);//allumer la led
				break ;
			}
			/*attente du flag RXNE*/}
		pdata[i] = USART1->RDR;//lire la valeur de l'octet recu
		}

}

//ISR appele chaque reception d'un nouveau caractere
void USART1_IRQHandler (void)
{
	static int i = 0;
	buffer_USART1[i] = USART1->RDR;//lire la valeur de l'octet recu
	i++;// incrementer le compteur
	nbChar_lidar_reception++;
	if(i == 5){

		//fin de reception d'un point
		flag_fin_reception_USART1 = 1;//indiquer la fin de reception d'un point
		i = 0;//reset le compteur
	}
	else if (i > 5) {
       // Sécurité : réinitialiser si dépassement
       i = 0;
    }

}

//indiquer la fin de reception d'un seul point
uint8_t fin_reception_USART1(void)
{
	if(flag_fin_reception_USART1 ){
		flag_fin_reception_USART1  = 0;//renitialiser le flag
		return 1;
	}
	else {
		return 0;
	}
}

//indiquer le nombre d'octet recu par le lidar
int Rplidar_nbChar_recived (void)
{
	return nbChar_lidar_reception++ ;
}

//stocker un point (5 octets)
void Rplidar_OnePoint_recived(uint8_t *pPoint)
{
	uint8_t i;
	for(i = 0;i<5 ; i++){
		pPoint[i] = buffer_USART1[i];
	}
}

//reset le buffer de usart1
void reset_buffer_USART1 (void)
{
	uint8_t i ;
	for(i = 0;i<5 ; i++){
		buffer_USART1[i] = 0;//reset le buffer de reception
	}

}

