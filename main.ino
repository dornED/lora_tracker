
#define LED GPIOC, GPIO_PIN_3
#define NSS GPIOA, GPIO_PIN_3

#define LED_ON()       GPIO_WriteLow(LED)
#define LED_OFF()      GPIO_WriteHigh(LED)
#define NSS_START()    GPIO_WriteLow(NSS)
#define NSS_STOP()     GPIO_WriteHigh(NSS)

#define DUMMY_BYTE   0

// send a single byte, and return received byte
static uint8_t SPI_SendByte(uint8_t data)
{
  // wait until transmit buffer is empty
  while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
  
  // send
  SPI_SendData(data);

  // wait until receive buffer is not empty
  while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  
  // return received byte
  return SPI_ReceiveData();
}

// write a single byte to an address
void SPI_Write(uint8_t addr, uint8_t data)  
{ 
  NSS_START();  
  SPI_SendByte(addr | 0x80);
  SPI_SendByte(data);  
  NSS_STOP();
}

// read a single byte from an address
uint8_t SPI_Read(uint8_t addr)
{
  NSS_START();
  SPI_SendByte(addr);                          
  uint8_t ret = SPI_SendByte(DUMMY_BYTE);
  NSS_STOP();  
  return ret;
}

// write multiple bytes to an address
void SPI_ReadBuffer(uint8_t addr, uint8_t *ptr, uint8_t length)
{
  NSS_START();
  SPI_SendByte(addr); 
  for (int i=0; i < length; ++i)
    ptr[i] = SPI_SendByte(DUMMY_BYTE);
  NSS_STOP(); 
}

// read multiple bytes from an address
void SPI_WriteBuffer(uint8_t addr, uint8_t *ptr, uint8_t length)
{
  NSS_START();  
  SPI_SendByte(addr | 0x80);
  for (int i=0; i < length; ++i)
    SPI_SendByte(ptr[i]);  
  NSS_STOP(); 
}


uint8_t received;

void setup()
{
  // init LED
  GPIO_Init(LED, GPIO_MODE_OUT_PP_LOW_SLOW);
  
  // init NSS pin
  GPIO_Init(NSS, GPIO_MODE_OUT_PP_HIGH_FAST);  
  NSS_STOP();

  // init SPI
  SPI_Init(SPI_FIRSTBIT_MSB, 
           SPI_BAUDRATEPRESCALER_2, 
           SPI_MODE_MASTER,
           SPI_CLOCKPOLARITY_LOW, 
           SPI_CLOCKPHASE_1EDGE, 
           SPI_DATADIRECTION_2LINES_FULLDUPLEX, 
           SPI_NSS_SOFT,
           0);
  SPI_Cmd(ENABLE);
  
  received = SPI_Read(0x06);
}

void loop()
{
  LED_ON();
  delay (500);
  LED_OFF();
  delay (500);

  // blink for every set bit
  for (int i=7; i>=0; --i)
  {
    if (received & (1 << i))
    {
      LED_ON();
      delay (200);
      LED_OFF();
      delay (200);
    }
    else
    {
      delay (400);
    }
  }
  
  LED_ON();
  delay (500);
  LED_OFF();
  delay (500);
}
