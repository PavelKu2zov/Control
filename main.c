#include "stm32f4xx.h"
#include "Init.h"

 void Parsing(uint32_t *size);
Circular_buffTypeDef Buff_USART1_RX;
char  BuffTxUsart1[SIZE_BUFF_DMA_UART1_TX];
char  BuffRxUsart1[SIZE_BUFF_DMA_UART1_RX];
PRMTypedef PRM;


float alfa,alfa_acc,alfa_g,alfa_old,alfa_gyro,alfa_gyro_old;
float Kp,Ki,Kd;
float I,P,D;//PID регул€тор
float e,e_old,u;// ошибка,уарпвл€ющий сигнал

uint32_t p_cndtr_tm;
uint32_t size_recieve_bytes;

void main()
{
 Init();
 alfa=alfa_acc=alfa_g=alfa_old=alfa_gyro=alfa_gyro_old=0;
 Kp = 11.5;//0.5;
 Ki = 0.1; 
 Kd = 280;//0.1
 P = 0; // пропорциональна€ составл€юща€
 I = 0; // интегральна€ 
 D = 0; // дифф.
 e_old = 0;
 
 Buff_USART1_RX.p_rd = BuffRxUsart1;
 Buff_USART1_RX.p_wr = BuffRxUsart1;
 Buff_USART1_RX.p_cndtr = &DMA2_Stream2->NDTR;
 Buff_USART1_RX.StartAdr = (char*)DMA2_Stream2->M0AR;
 Buff_USART1_RX.Size = SIZE_BUFF_DMA_UART1_RX;
 
for(;;)
{
   /*-------------------прием данных по UART1-------------------------------*/
  p_cndtr_tm = *Buff_USART1_RX.p_cndtr;
if (( Buff_USART1_RX.Size - p_cndtr_tm) >= (uint32_t)(Buff_USART1_RX.p_rd - Buff_USART1_RX.StartAdr))
     size_recieve_bytes = (Buff_USART1_RX.Size - p_cndtr_tm) - 
                              (uint32_t)(Buff_USART1_RX.p_rd - Buff_USART1_RX.StartAdr);
else
     size_recieve_bytes = (Buff_USART1_RX.Size - (uint32_t)(Buff_USART1_RX.p_rd - Buff_USART1_RX.StartAdr))+
                             (uint32_t)(Buff_USART1_RX.Size - p_cndtr_tm);

  if ( size_recieve_bytes != 0) 
	Parsing(&size_recieve_bytes);


}

 
}