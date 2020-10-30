#include "stm32f4xx.h"

 int8_t ReadDataSPI1(uint8_t reg_addr, uint8_t *data, uint16_t len);
 int8_t WriteDataSPI1(uint8_t reg_addr, uint8_t data);