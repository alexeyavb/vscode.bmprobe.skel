#include "circular_buffer.h"
#include "i2s.h"
#include "dma.h"
#include "tusb_config.h"

static inline void MX_NVIC_Init(void){
  HAL_NVIC_SetPriority(FPU_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(FPU_IRQn);
  
  HAL_NVIC_SetPriority(SPI2_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(SPI2_IRQn);
  
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void board_init_after_tusb(void){
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  MX_DMA_Init();
  MX_I2S2_Init();  
  MX_NVIC_Init();

  extern void AX_InitCircualBuffer(void);
  AX_InitCircualBuffer();
  AX_InitDmaBuffer();
  // HAL_I2S_Receive_DMA(&hi2s2, (uint16_t*)(&(paudio_dma_rcv_buffer->ONE_SAMPLE_DATA_16[0])), paudio_dma_rcv_buffer->buffer_sample_size);
}