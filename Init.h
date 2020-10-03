#include "stm32f4xx.h"

#define  SIZE_BUFF_DMA_SPI1     16
#define  SIZE_BUFF_DMA_SPI2     16
#define  SIZE_BUFF_DMA_UART1_TX     32
#define  SIZE_BUFF_DMA_UART1_RX     128

#define  SPI1_DR_8bit         *(__IO uint8_t*)&(SPI1->DR)


#define  delta_t                 (float)(0.02)
#define  PI                      (float)3.1415926535
#define  K                       (float)0.01             //����.��������������� �������
#define  fi                      (float)0                //���� ���������

typedef struct 
{
  char     	          *p_rd;                   // ��������� �� ��������� ����� �� �����
  char                *p_wr;       			   // ��������� �� ��������� ����� �� ������
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
  
  
  
  
int8_t ReadDataSpi1(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t WriteDataSpi1(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
void  TransmitPRM_BLDC(uint16_t g);
void  Init(void);