#ifndef ROBOT_COMMUNICATION_H
#define ROBOT_COMMUNICATION_H
#include <stdint.h>

	void USART3_init(void);
	void USART3_send_char(uint8_t message);
	uint8_t USART3_recieve_char(void);
	uint8_t Fin_reception_Usart3 (void);
	void Usart3_recieved_Instructions(uint8_t * pDistance, int16_t * pAngle );
	uint8_t nbInstructions_recu (void);
	void reset_buffer_USART3(void);
	/*************************/
	void USART1_init(void);
	void USART1_Interruption_Inable(void);
	void USART1_Interruption_Disable(void);
	void USART1_send_char(uint8_t message);
	void USART1_char_recieved(uint8_t *pdata , uint8_t nbdata);
	uint8_t fin_reception_USART1(void);
	int Rplidar_nbChar_recived(void);
	void Rplidar_OnePoint_recived(uint8_t *pPoint);
	void reset_buffer_USART1 (void);


#endif
