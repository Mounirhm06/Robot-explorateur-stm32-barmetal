/* Développé par HAMMAMI Mounir*/
#include <stdio.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "i2c.h"
#include "Robot_MPU6050.h"

/******** registres du MPU6050 *************************/
uint8_t MPU_adrr = (0x68<<1);//si ADO = 1 => adresse devient 0x69
uint8_t WHO_AMI_Reg_adrr = 0x75;//ce registre contient la valeur de l'adresse de MPU6050
uint8_t PWR_MGMT_1_adrr = 0x6B;//configurer le sleep mode,l'horloge de MPU,le cycle ..
uint8_t SMPRT_DIV = 0x19;//choisir la division d'horloge (Sample Rate)
uint8_t GYRO_CONFIG_REG = 0x1B;  // registre de configuration du gyroscope(plages de mesures)
uint8_t GYRO_XOUT_H = 0x43;//contient les MSB de la mesure de Gyro, les autres registres sont successif => pointer sur ce registre permet de lire toutes les mesures de gyro
/******************************************************/
float yawVal = 0;

/*Init dU MPU6050*/
void MPU6050_Init(void)
{
	uint8_t check_adr = 0;//si le capteur est connecte au bus il renvoie son adresse 0x68(104 dec) via le bus
	uint8_t commande[2] = {0};//commande envoyees au capteur
	//tester si le capteur est connecte au bus
	HAL_I2C_Master_Transmit(&hi2c1,MPU_adrr,&WHO_AMI_Reg_adrr,1,100);
	HAL_I2C_Master_Receive(&hi2c1,MPU_adrr,&check_adr,1,100);//lire le registre WHO_AM_I
	if(check_adr == 104){//capteur bien detecter
		//renitialiser les registres de MPU
		commande[0] = PWR_MGMT_1_adrr;
		commande[1] =  0x80;//bit Reset = 1
		HAL_I2C_Master_Transmit(&hi2c1,MPU_adrr,commande,2,100);
		HAL_Delay(100); // attente de la fin de la renitialisation

		//quitter le mode sleep,disactiver le capteur de temperature(TEMP_DIS), choisir clock = 8Mhz
		commande[1] = 0x08;
		HAL_I2C_Master_Transmit(&hi2c1,MPU_adrr,commande,2,100);

		//choix de la frequence d'echantillonage (Sample Rate)1 KHz
		commande[0] = SMPRT_DIV;
		commande[1] =  0x27;//SMPLRT_DIV= 39 => Sample Rate = 8Khz/(39+1) = 200Hz (200 mesures par seconde)
		HAL_I2C_Master_Transmit(&hi2c1,MPU_adrr,commande,2,100);

		// Configurer le gyroscope pour ±250°/s, bit FS_SEL
		commande[0] = GYRO_CONFIG_REG;
		commande[1] = 0x00; // ±250°/s
		HAL_I2C_Master_Transmit(&hi2c1, MPU_adrr, commande, 2, 100);


	}

}

/*receptions des donnes de Gyro*/
void Gyro_mesures(float *pGyro_mesur)
{
	uint8_t gyro_mesur[6] = {0};//stocker les valeurs brutes recu du gyro
	int16_t Xgyro = 0;
	int16_t Ygyro = 0;
	int16_t Zgyro = 0;

	//recuperer les mesures brutes
	HAL_I2C_Master_Transmit(&hi2c1,MPU_adrr,&GYRO_XOUT_H,1,100);// demande d'envoie des mesures de Gyro
	HAL_I2C_Master_Receive(&hi2c1,MPU_adrr,gyro_mesur,6,100);//lire les valeurs brutes

	//transformer les valeurs en 16bits (MSB,LSB)
	 Xgyro =(gyro_mesur[0]<<8) | gyro_mesur[1];
	 Ygyro =(gyro_mesur[2]<<8) | gyro_mesur[3];
	 Zgyro =(gyro_mesur[4]<<8) | gyro_mesur[5];

	// transformer les valeurs brutes en degree
	//plage de 500[-250,250] avec 16 bits => quantum = 500/(2^16)-1 = 1/131
	pGyro_mesur[0] = (Xgyro +0.0 )/131.0;//X_Gyro en degree
	pGyro_mesur[1] = (Ygyro +0.0)/131.0;//Y_Gyro en degree
	pGyro_mesur[2] = (Zgyro +0.0)/131.0;//Z_Gyro en degree
}

/*Calcul de l'angle de rotation du robot/l'axe Z (YAW)*/
float yaw_value(float *pGyro_mesures,uint32_t tempsTi,uint32_t tempsTiPlus)
{
	float deltaT = 0;
	deltaT = (float)(tempsTiPlus - tempsTi) /1000.0;//temps ecoule entre deux mesures en s
	yawVal = yawVal + pGyro_mesures[2] * deltaT;//calcul de la nouvelle valeur de yaw
	return yawVal;
}

void Reset_yawVal(void)
{
	/*pour eviter la derive du capteur il faut renitialiser a chaque fin de consigne d'angle*/
	yawVal = 0;
}

