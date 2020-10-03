#include "stm32f10x.h"
typedef struct 
{
	uint16_t analog_ch[16];
	uint8_t  dig_ch[2];	
}S_BUSTypeDef;  

typedef enum                              // статус приема 
  {     
    SEARCH_MARKER_START,                  // поиск маркера MARKER_START
    RECEIVE_DATA,                         // прием пакета 
    SEARCH_MARKER_END                     // поиск маркера MARKER_END
  }StateReceiveTypeDef;


typedef struct
{
  StateReceiveTypeDef State;
  uint16_t AmountReceiveDataByte;
}StatusReceiveTypeDef;


void CreateSbusFrame(S_BUSTypeDef *data, char *buff);