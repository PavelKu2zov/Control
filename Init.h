#include "stm32f4xx.h"

#define  SIZE_BUFF_DMA_SPI1     32
#define  SIZE_BUFF_DMA_SPI2     32
#define  SIZE_BUFF_DMA_UART1_TX     128
#define  SIZE_BUFF_DMA_UART1_RX     128

#define  SPI3_DR_8bit         *(__IO uint8_t*)&(SPI3->DR)
#define  SPI1_DR_8bit         *(__IO uint8_t*)&(SPI1->DR)

#define  delta_t                 (float)(0.02)
#define  PI                      (float)3.1415926535
#define  K                       (float)0.01             //коэф.комплеметарного фильтра
#define  fi                      (float)0                //угол установки

typedef struct 
{
  char     	      *p_rd;                   // указатель на кольцевой буфер дл¤ чтени¤
  char                *p_wr;       	       // указатель на кольцевой буфер дл¤ записи
  volatile uint32_t   *p_cndtr;                // указатель на инверсный счетчик принимаемых данных(используетс¤ DMA)
  char                *StartAdr;               // указатель на стартовый адрес кольцевого буфера
  uint32_t            Size;                    // размер кольцевого буфера типа uint8_t
}Circular_buffTypeDef;


enum  MODECOM          // режим  СОМ порта
  {     
    StopUART,            // Stop передачи из буфера
	BeginUART,           // начало приема или передачи пакета  с данными
	WorkUART,            // прием или передачa пакета  с дааными
	End_0x10,            // конeц пакета -предпоследний байт
	End_0x03,            // конeц пакета - последний байт
	End,                 // конeц пакета - буфер еще не опустошен !
	Simb_0x10,           // служебный байт
  };
  
typedef struct	            // прием пакета по СОМ порту
  {	
    enum  MODECOM   Mode;       // состояние приема
	uint32_t   	ByteCounter;  // длина пакета
	unsigned char	*p;           // указатель на буфер
	uint8_t Flag_Simb_0x10;		//наличие символа 0x10
	uint32_t	RxTimout;     // таймаут по приему
  }UartStreamTypeDef;

typedef struct
{
  int8_t GasValue;
  uint16_t Thrust;
}PRMTypedef;
  


typedef struct
{
 float alfa,alfa_acc,alfa_g,alfa_old,alfa_gyro,alfa_gyro_old;
 float Kp,Ki,Kd;
 float I,P,D;//PID регулятор
 float e,e_old,u;// ошибка,управляющий сигнал
 float angleAccel_x;//угол по акселерометру
 float angleAccel_y;
 float angleAccel_z;
 float angleGyro_x;//угол по гироскопу
 float angleGyro_y;
 float angleGyro_z;
}PIDTypeDef;

int8_t ReadDataSpi3(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t WriteDataSpi3(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
void TimerBmi160(uint32_t delay);
void  TransmitPRM_BLDC(uint16_t g);
void  Init(void);
void  SPI_SetBaudRatePrescaler(SPI_TypeDef* SPIx, uint16_t BaudRatePrescaler);