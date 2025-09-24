#include "stm32l4xx.h"
#include "main.h"
#include "Robot_communication.h"
#include "Robot_Lidar.h"
#include "Robot_voyants.h"

/************** communication avec le lidar et mesures *********************/

// cette fonction retourne l'etat du lidar et le message d'erreur qui va avec
uint8_t Rplidar_health_statut(uint16_t *error_msg)
{
	uint8_t data[7] = {0};
	uint8_t message_erreur[3] = {4,0,4};
	USART1_send_char(RPLIDAR_CMD_SYNC_BYTE);//bit de synchroisation
	USART1_send_char(RPLIDAR_CMD_GET_HEALTH);//demander l'etat du lidar
	USART1_char_recieved(data,7);//reception du Descriptor
	USART1_char_recieved(message_erreur,3);//reception des data(etat du lidar)
	*error_msg = (message_erreur[1]<<8)|message_erreur[2];
	 if(message_erreur[0] == 0){
		return 0;//status byte  = 0 ,GOOD
	 }
	 else if(message_erreur[0] == 1){
		 return 1;//status byte  = 1,WARNING
	 }
	 else {
		 return 2;//status byte  = 2, ERROR
	 }
}

//renitialiser le lidar en cas d'erreur , retour en etat idle
void Rplidar_Reset(void)
{
	USART1_send_char(RPLIDAR_CMD_SYNC_BYTE);//bit de synchroisation
	USART1_send_char(RPLIDAR_CMD_RESET);//Reset
	HAL_Delay(2);//2ms avant la prochaine requet
}

//Initilisation du lidar
uint8_t Rplidar_Init(void)
{
	uint16_t error_msg = 0;
	if(Rplidar_health_statut(&error_msg) == 0){
		//lidar en bonne etat on peut commencer le scann
		return 1;
	}
	else{
		//warning ou ERROR
		Rplidar_Reset();//faire un reset
		//demander a nouveau l'etat du lidar
		if(Rplidar_health_statut(&error_msg) == 0){
			return 1;// le lidar remarche
		}
		else {
			LED_Lidar_erreur();//LED rouge ON
			return 0; // erreur arreter le robot
		}
	}
}

//commencer le scan
uint8_t Rplidar_Start_Scan(uint8_t *data)
{
	USART1_send_char(RPLIDAR_CMD_SYNC_BYTE);//bit de synchroisation
	USART1_send_char(RPLIDAR_CMD_START_SCAN );//commande de scan
	USART1_char_recieved(data,7);//reception du Descriptor
	if(data[2] == 0x5){
		return 1;//debut du scan avec succes
	}
	return 0;
}

//arreter le scan
void Rplidar_Stop_Scan(void)
{
	USART1_send_char(RPLIDAR_CMD_SYNC_BYTE);
	USART1_send_char(RPLIDAR_CMD_STOP);
	HAL_Delay(1);//delay de 1ms
}


//mesurer N points
 void Rplidar_mesures(uint8_t *pPoints ,uint16_t nbPoints )
{
	uint16_t i = 0;
	uint8_t desconter[7] = {0};
	if(Rplidar_Init()){
	//le lidar fonctionne sans erreur
		if( Rplidar_Start_Scan(desconter)){
			USART1_Interruption_Inable();//activer la reception par interruption
			//reception des nbPoints
			do{
				if(fin_reception_USART1()){
					// si nouveau point est recu
					Rplidar_OnePoint_recived(&pPoints[i*5]);//lire le point et le stocker dans la ligne i de la matrice points
					i++;//incrementer le nombre de points recu
					reset_buffer_USART1();//reset le buffer de reception apres chauqe point
				}
			}while(i<nbPoints);
			Rplidar_Stop_Scan();
			USART1_Interruption_Disable();//disactiver la reception
		}
	}
}

 //transformer les mesures en Angle et , pour le test sur Putty
 void Rplidar_map(uint8_t *ppoint, float *Angle, float *Distance, uint16_t nbpoints)
 {
     uint16_t angle_q6, distance_q2; // Variables temporaires pour stocker les valeurs intermédiaires
     uint16_t i, index;

     for (i = 0; i < nbpoints; i++) {
         index = i * 5; // Chaque point occupe 5 octets

         // Transformer l'angle et la distance en 16 bits
         angle_q6 = ((ppoint[index + 1] >> 1) & 0x7F) | (ppoint[index + 2] << 7);
         distance_q2 = ppoint[index + 3] | (ppoint[index + 4] << 8);

         // Transformer les valeurs en mesures réelles
         Angle[i] = angle_q6 / 64.0; // Octets 2 et 3 pour l'angle, angle en °
         Distance[i] = distance_q2 / 4.0; // Octets 4 et 5 pour la distance, distance en mm
     }
 }


/****************************** moteur lidar speed  ***********************************/
//initialiser le timer utiliser pour generer le PWM
void Tim3_Init (void)
{

		 RCC->APB1ENR1 |= (1<<1);//Timer TIM3 enable
		 TIM3->PSC = 3 ; //4MHZ/4 = 1MHz (1 tick par 1us)
		 TIM3->ARR =  599; // overflow a 150us=> frequence PWM 6.66Khz
		 TIM3->CR1 |=(1<<0);//enable timer bit CEN/
		 TIM3->SR &= ~(1<<0);//renitialiser le flag UIF*/
		 TIM3->CNT = 0; //renitialiser le compteur
}

// configrer le timer en mode PWM
void PWM_TIM3_CH1_config (void)
{
	// chanel 1 TIM3, pin PB4
	TIM3->CCR1 = 00; // rapport cyclique de 00/20
	TIM3->CCMR1 &= ~(0x7<<4); // effacer les bits OC1M, permet de definir la configuration du chanel
	TIM3->CCMR1 |= (0x6<<4); // PWM (si CNT<=CCR => sortie up si CNT=>CRR => sortie down)
	TIM3->CCMR1 |= (1<<3); // activation d'autoreload à chaque update event(bit OC1PE), permet de changer la valeur de PWM
	TIM3->CCER &= ~(1<<1); // polarity 0 (bit CC1P), logique positive
	TIM3->CCER |= (1<<0); // activer le mode compare sur le canal1(bit CC1E)
}

// initialiser le pin PB4 en mode Alternatif
void Rplidar_PWM_Pin_Init(void)
{
	RCC->AHB2ENR |= (1<<1);//enabble GPIOB
	GPIOB->MODER &= ~(0x3<<8); //effacer les bits MODE4
	GPIOB->MODER |= (0x2<<8); // mode alternatif
	GPIOB->OTYPER &= ~(1<<4); // push-pull
	GPIOB->OSPEEDR &= ~(0x3<<8);//Low speed
	GPIOB->AFR[0] &= ~(0xF<<16);//effacer les bits AFSEL4
	GPIOB->AFR[0] |= (0x2<<16);//fonction AF2(TIM3_CH1)

}

//choix de la vitesse pwm du moteur du lidar
void RPLIDAR_SetMotorPWM(uint16_t speedL)
{
	TIM3->CCR1 = speedL;
}

//Initialiser les PWM d'un coup
void Lidar_PWM_Init(void)
{
	 Tim3_Init();
	 PWM_TIM3_CH1_config();
	 Rplidar_PWM_Pin_Init();
}
