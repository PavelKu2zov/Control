#include "Init.h"
#include "Sbus.h"

#define MARKER_START	        (char)0xf0
#define MARKER_END		(char)0x00


/* Создает  S-BUS frame.
*  data - входные данные
*  *buff - указатель на буффер для передачи по usart
*/

void CreateSbusFrame(S_BUSTypeDef *data, char *buff)
{
	*buff =  MARKER_START;
   buff++;   
	*buff = 0xe8;//(data->analog_ch[0])&0xff;
   buff++;
	*buff = 0x03;//((data->analog_ch[0]>>8)|(data->analog_ch[1]<<3))&0xff;
        return;
   buff++;                                                                      
	*buff = ((data->analog_ch[1]>>5)|(data->analog_ch[2]<<6))&0xff;
   buff++;
	*buff = (data->analog_ch[2]>>2)&0xff;
   buff++;
	*buff = ((data->analog_ch[2]>>10)|(data->analog_ch[3]<<1))&0xff;
   buff++;
	*buff = ((data->analog_ch[3]>>7)|(data->analog_ch[4]<<4))&0xff;
   buff++;
	*buff = ((data->analog_ch[4]>>4)|(data->analog_ch[5]<<7))&0xff;
   buff++;
	*buff = (data->analog_ch[5]>>1)&0xff;
   buff++;
	*buff = ((data->analog_ch[5]>>9)|(data->analog_ch[6]<<2))&0xff;
   buff++;
	*buff = ((data->analog_ch[6]>>6)|(data->analog_ch[7]<<5))&0xff;
   buff++;
	*buff = (data->analog_ch[7]>>3)&0xff;
   buff++;

   *buff = (data->analog_ch[8])&0xff;
   buff++;
	*buff = ((data->analog_ch[8]>>8)|(data->analog_ch[9]<<3))&0xff;
   buff++;
	*buff = ((data->analog_ch[9]>>5)|(data->analog_ch[10]<<6))&0xff;
   buff++;
	*buff = (data->analog_ch[10]>>2)&0xff;
   buff++;
	*buff = ((data->analog_ch[10]>>10)|(data->analog_ch[11]<<1))&0xff;
   buff++;
	*buff = ((data->analog_ch[11]>>7)|(data->analog_ch[12]<<4))&0xff;
   buff++;
	*buff = ((data->analog_ch[12]>>4)|(data->analog_ch[13]<<7))&0xff;
   buff++;
	*buff = (data->analog_ch[13]>>1)&0xff;
   buff++;
	*buff = ((data->analog_ch[13]>>9)|(data->analog_ch[14]<<2))&0xff;
   buff++;
	*buff = ((data->analog_ch[14]>>6)|(data->analog_ch[15]<<5))&0xff;
   buff++;
	*buff = (data->analog_ch[15]>>3)&0xff;
   buff++;  

  *buff = data->dig_ch[0] ? (*buff | 0x80) : (*buff & (~0x80));
  *buff = data->dig_ch[1] ? (*buff | 0x40) : (*buff & (~0x40));
   buff++;
  *buff = MARKER_END;
  
}



/*
------------------------------ Разбор пакета S_bus-----------------------------------      
Circular_buffTypeDef:
     - uint8_t     	*p;                 // указатель на кольцевой буфер
     - uint32_t          *StartAdr;          // указатель на стартовый адрес кольцевого буфера
     - uint32_t         Size;               // размер кольцевого буфера типа uint8_t

return -1 if was error, 0 if wasn't full data, 1 received full data
	 
*/
int Parsing_S_BUS( Circular_buffTypeDef *pCircular_buff, S_BUSTypeDef *data)
{ 
  static StatusReceiveTypeDef StatusReceive = {{SEARCH_MARKER_START},0};
  static uint8_t AmountDataBits = 0;
  static uint16_t tempData=0;
  static uint8_t NumCh=0;
  char ch;
  
 uint32_t p_cndtr_tm;
 uint16_t size_read_bytes = 0;

 p_cndtr_tm = *pCircular_buff->p_cndtr;
 size_read_bytes = 0;
    if ((pCircular_buff->Size - p_cndtr_tm) >= (uint32_t)(pCircular_buff->p_rd - pCircular_buff->StartAdr))
      size_read_bytes = (pCircular_buff->Size - p_cndtr_tm) - 
                              (uint32_t)(pCircular_buff->p_rd - pCircular_buff->StartAdr);
    else
      size_read_bytes = (pCircular_buff->Size - (uint32_t)(pCircular_buff->p_rd - pCircular_buff->StartAdr))+
                             (uint32_t)(pCircular_buff->Size - p_cndtr_tm);
  
  
  while ( size_read_bytes )
      {
		  ch = *pCircular_buff->p_rd;
		 if (pCircular_buff->p_rd == (pCircular_buff->StartAdr + pCircular_buff->Size))
			 pCircular_buff->p_rd = pCircular_buff->StartAdr;
		 else
			 pCircular_buff->p_rd++;
		 
                switch ( StatusReceive.State )
                {
                  case SEARCH_MARKER_START: 
                            if ( ch == MARKER_START )
                              {
                                StatusReceive.State = RECEIVE_DATA;
								StatusReceive.AmountReceiveDataByte = 0;
								tempData = 0;
								AmountDataBits = 0;
                              }
                            break;
                  case RECEIVE_DATA: 
                            if (StatusReceive.AmountReceiveDataByte < 22)//аналоговые каналы
                              {
                                tempData |= ((uint16_t )ch<<AmountDataBits);
                                AmountDataBits+=8;
                                  
                                if (AmountDataBits >= 11)
                                  {
                                     data->analog_ch[NumCh++] = tempData & 0x07ff; 
                                     AmountDataBits-=11;
                                     tempData = tempData >> 11;
                                  }
                              }
                                  else if (StatusReceive.AmountReceiveDataByte == 22)//цифровые каналы
                                  {
                                        data->dig_ch[0] = (ch & 0x80)>>7;
                                        data->dig_ch[1] = (ch & 0x40)>>6;
                                        StatusReceive.State = SEARCH_MARKER_END;
                                  }
				StatusReceive.AmountReceiveDataByte++;
                            break;                               
                  case SEARCH_MARKER_END:
							StatusReceive.State = SEARCH_MARKER_START;
                            if (ch == MARKER_END)
								return 1;
							else
								return -1;
                                    
							break;
                           
                }
       size_read_bytes--;
      }
return 0;
}


