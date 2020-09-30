#include "Init.h"
#include "Sbus.h"

#define MARKER_START	        (char)0xf0
#define MARKER_END		(char)0x00


/*	Создает  S-BUS frame.
	data - входные данные
        *buff - указатель на буффер для передачи по usart

*/

void CreateSbusFrame(S_BUSTypeDef *data, char *buff)
{
  *buff =  MARKER_START;
   buff++;
  *buff = (data->analog_ch[0])&0xff;
   buff++;
	*buff = ((data->analog_ch[0]>>8)|(data->analog_ch[1]<<3))&0xff;
   buff++;                                                                      //далее поправить
	*buff = ((data->analog_ch[1]<<2)|(data->analog_ch[2]>>9))&0xff;
   buff++;
	*buff = (data->analog_ch[2]>>1)&0xff;
   buff++;
	*buff = ((data->analog_ch[2]<<7)|(data->analog_ch[3]>>4))&0xff;
   buff++;
	*buff = ((data->analog_ch[3]<<4)|(data->analog_ch[4]>>7))&0xff;
   buff++;
	*buff = ((data->analog_ch[4]<<1)|(data->analog_ch[5]>>10))&0xff;
   buff++;
	*buff = (data->analog_ch[5]>>2)&0xff;
   buff++;
	*buff = ((data->analog_ch[5]<<6)|(data->analog_ch[6]>>5))&0xff;
   buff++;
	*buff = ((data->analog_ch[6]<<3)|(data->analog_ch[7]>>8))&0xff;
   buff++;
	*buff = (data->analog_ch[7])&0xff;
   buff++;

   *buff = (data->analog_ch[8]>>3)&0xff;
   buff++;
	*buff = ((data->analog_ch[8]<<5)|(data->analog_ch[9]>>6))&0xff;
   buff++;
	*buff = ((data->analog_ch[9]<<2)|(data->analog_ch[10]>>9))&0xff;
   buff++;
	*buff = (data->analog_ch[10]>>1)&0xff;
   buff++;
	*buff = ((data->analog_ch[10]<<7)|(data->analog_ch[11]>>4))&0xff;
   buff++;
	*buff = ((data->analog_ch[11]<<4)|(data->analog_ch[12]>>7))&0xff;
   buff++;
	*buff = ((data->analog_ch[12]<<1)|(data->analog_ch[13]>>10))&0xff;
   buff++;
	*buff = (data->analog_ch[13]>>2)&0xff;
   buff++;
	*buff = ((data->analog_ch[13]<<6)|(data->analog_ch[14]>>5))&0xff;
   buff++;
	*buff = ((data->analog_ch[14]<<3)|(data->analog_ch[15]>>8))&0xff;
   buff++;
	*buff = (data->analog_ch[15])&0xff;
   buff++;  
   
  *buff = 0x00;
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


