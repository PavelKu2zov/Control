#include "stm32f4xx.h"

#define  SIZE_BUFF_DMA_SPI1     32
#define  SIZE_BUFF_DMA_SPI2     32
#define  SIZE_BUFF_DMA_UART1_TX     128
#define  SIZE_BUFF_DMA_UART1_RX     128

#define  SPI3_DR_8bit         *(__IO uint8_t*)&(SPI3->DR)
#define  SPI1_DR_8bit         *(__IO uint8_t*)&(SPI1->DR)

#define  delta_t                 (float)(0.02)
#define  PI                      (float)3.1415926535
#define  K                       (float)0.01             //����.��������������� �������
#define  fi                      (float)0                //���� ���������

typedef struct 
{
  char     	      *p_rd;                   // ��������� �� ��������� ����� �� �����
  char                *p_wr;       	       // ��������� �� ��������� ����� �� ������
  volatile uint32_t   *p_cndtr;                // ��������� �� ��������� ������� ����������� ������(����������� DMA)
  char                *StartAdr;               // ��������� �� ��������� ����� ���������� ������
  uint32_t            Size;                    // ������ ���������� ������ ���� uint8_t
}Circular_buffTypeDef;


enum  MODECOM          // �����  ��� �����
  {     
    StopUART,            // Stop �������� �� ������
	BeginUART,           // ������ ������ ��� �������� ������  � �������
	WorkUART,            // ����� ��� �������a ������  � �������
	End_0x10,            // ���e� ������ -������������� ����
	End_0x03,            // ���e� ������ - ��������� ����
	End,                 // ���e� ������ - ����� ��� �� ��������� !
	Simb_0x10,           // ��������� ����
  };
  
typedef struct	            // ����� ������ �� ��� �����
  {	
    enum  MODECOM   Mode;       // ��������� ������
	uint32_t   	ByteCounter;  // ����� ������
	unsigned char	*p;           // ��������� �� �����
	uint8_t Flag_Simb_0x10;		//������� ������� 0x10
	uint32_t	RxTimout;     // ������� �� ������
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
 float I,P,D;//PID ���������
 float e,e_old,u;// ������,����������� ������
 float angleAccel_x;//���� �� �������������
 float angleAccel_y;
 float angleAccel_z;
 float angleGyro_x;//���� �� ���������
 float angleGyro_y;
 float angleGyro_z;
}PIDTypeDef;

int8_t ReadDataSpi3(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t WriteDataSpi3(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
void TimerBmi160(uint32_t delay);
void  TransmitPRM_BLDC(uint16_t g);
void  Init(void);
void  SPI_SetBaudRatePrescaler(SPI_TypeDef* SPIx, uint16_t BaudRatePrescaler);