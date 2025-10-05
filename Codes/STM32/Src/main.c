/*Développer par HAMMAMI Mounir 20/04/2025*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stm32l4xx.h"
#include "main.h"
#include "i2c.h"
#include "Timers.h"
#include "Robot_Communication.h"
#include "Robot_control_direction.h"
#include "Robot_encodeurs.h"
#include "Robot_MPU6050.h"
#include "Robot_Lidar.h"
#include "Robot_voyants.h"

void SystemClock_Config(void);
void LED_PC7_Init (void);



//uint8_t message_recu[2] = {0};
int main(void)
{
	uint8_t j,k;
	uint16_t i;

	float Gyro_mesur[3];
	float yawValeur = 0;
	int16_t angle_consigne = 0;
	uint32_t temps_ti = 0;//le temps a teta_i
	uint32_t temps_tiPlus = 0;//le temps a teta_i+1
	uint8_t points[1000][5] = {0};//matrice pour stocker 1000 points du lidar (chaque point est compose de 5 octets)
	/*les valeurs en ASCII*/
	uint8_t Distance_instruction[20] = {0};
	int16_t Angel_instruction[20] = {0};

	HAL_Init();
	MX_I2C1_Init();

	/*Init horloge et timers*/
	SystemClock_Config();
	Tim5_Init();

	/*intit control mouvement */
	Moteurs_Init();
	Robot_PWM_Init();

	/*Init capteurs*/
	Robot_encodeurs_Init();
	MPU6050_Init();
	Lidar_PWM_Init();

	/*Init communication*/
	USART3_init();
	USART1_init();

	/*Init les voyants */
	Voyants_Init();


	RPLIDAR_SetMotorPWM(500);//lancer le moeteur(300 preferable)
	delay_ms(100);//delay de 100ms pour stabiliser la vitesse du moteur


	while(1)
	{


		/****************** commande wifi ********************/
		if(Fin_reception_Usart3()){
			LED_New_commande_reception();
			Usart3_recieved_Instructions(Distance_instruction,Angel_instruction);//lire les instructions recu  du PC
		    Angle_transformation(Angel_instruction,nbInstructions_recu());//transfomer l'angle en -180 <angle< 180
			for(i = 0 ;i<nbInstructions_recu();i++ ){

				//suivre les instructions de rotation et d'avancement pour chaque instruction
				renitialiser_distancd_M2();//renitialiser l'encodeur

				Reset_yawVal();//renitialiser la mesure du MPU6050(le nouveau point devient la reference pour le prochain)
				//choix du sens de rotation
				angle_consigne = Angel_instruction[i];//lire la valeur de la consigne
				if(angle_consigne>=0){
					// si l'angle est positif on tourne a droite [0;180]
					robot_rotation_control(RIGHT,40);
				}
				else {
					// si l'angle est negatif on tourne a gauche [-180:0]
					robot_rotation_control(LEFT,40);
				}

			    //tourner vers l'angle consigne,
				do{
					temps_ti = temps();//recuperer le temps a l'instant Ti en ms
					//il faut commencer par tourner pour avoir une valeur du Gyro non nulle(car on mesure la vitesse angulaire)
					 Gyro_mesures(Gyro_mesur);//recuperer les mesures de gyro
					// HAL_Delay(10);//lire la mesure chaque 10ms
					 delay_ms(10);
				     temps_tiPlus = temps();//recuperer la valeur du temps a l'instant Ti+1 en ms
					 yawValeur = yaw_value(Gyro_mesur,temps_ti,temps_tiPlus)*1.05;//calcul de l'angle de rotation
				}while(fabs(yawValeur) <=abs(angle_consigne));

				Robot_Direction_control(STOP,0);//arreter les moteurs
				delay_ms(500);///

				//renitialiser les encodeurs
				renitialiser_distancd_M2();

				// avancer vers la distance consigne
				// chaque tour represente 86  impulsions d'encodeurs
				while(distance_parcou_M2()<=Distance_instruction[i]*86){
					Robot_Direction_control(FORWARD,40);
				}

			}
			Robot_Direction_control(STOP,0);//arreter les moteurs
			reset_buffer_USART3();//pour recevoir les nouvelles instructions
			delay_ms(100);//contre l'enertie des moteur

			// les mesures du lidar
			Rplidar_mesures(&points[0][0],1000);
			//envoyer les mesures au module wifi esp8266
			for (i = 0; i < 1000; i += 10) { // Parcourir les points par blocs de 10
			    for (j = 0; j < 10; j++) {  // Parcourir les 10 points du paquet
			         for (k = 0; k < 5; k++) { // Parcourir les 5 octets de chaque point
			             USART3_send_char(points[i + j][k]); // Envoyer chaque octet
			         }
			    }
			    delay_ms(10);//10ms apres chaque paquet de 50octets  pour la syncronisation de l'esp8266
			}
			reset_buffer_USART1();//pour envoyer des nouvels points*/
		}

		else {
			// si aucune instructions est recu le robot se met a l'arret
			Robot_Direction_control(STOP,0);//arreter les moteurs
		}
		
	}


	return 0;
}




 /* Configure le système pour utiliser MSI à 4 MHz */
 void SystemClock_Config(void)
 {
     /* Activer l’oscillateur MSI */
     RCC->CR |= RCC_CR_MSION;
     /* Attendre que MSI soit prêt */
     while ((RCC->CR & RCC_CR_MSIRDY) == 0) {}

     /*
      * Configurer la plage de MSI à 4 MHz.
      * Pour STM32L4, les bits MSIRANGE (bits [7:4] dans RCC->CR) doivent être réglés sur 0110 pour 4 MHz.
      */
     RCC->CR &= ~(0xF << RCC_CR_MSIRANGE_Pos);   // Effacer les bits MSIRANGE
     RCC->CR |= (6 << RCC_CR_MSIRANGE_Pos);        // Plage = 6 -> 4 MHz

     /* Sélectionner MSI comme source de l'horloge système (SW bits à 00) */
     RCC->CFGR &= ~RCC_CFGR_SW;  // Remise à zéro des bits SW (0 = MSI)

     /* Attendre que MSI soit effectivement utilisé comme source système */
     while ((RCC->CFGR & RCC_CFGR_SWS) != 0) {}  // SWS doit être 00 pour MSI
 }











/******************   fonction CUBIMIX **********************/
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


