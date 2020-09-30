#include "stm32f10x.h"
typedef struct 
{
	uint16_t analog_ch[16];
	uint8_t  dig_ch[2];	
}S_BUSTypeDef;  

typedef enum                              // ������ ������ 
  {     
    SEARCH_MARKER_START,                  // ����� ������� MARKER_START
    RECEIVE_DATA,                         // ����� ������ 
    SEARCH_MARKER_END                     // ����� ������� MARKER_END
  }StateReceiveTypeDef;


typedef struct
{
  StateReceiveTypeDef State;
  uint16_t AmountReceiveDataByte;
}StatusReceiveTypeDef;


void CreateSbusFrame(S_BUSTypeDef *data, char *buff);