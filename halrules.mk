CFLAGS_GCC += -Wno-error=redundant-decls \
    -Wno-error=unused-parameter \
    -Wno-error=cast-qual \
    -Wno-redundant-decls \


SRC_C += \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_ll_usb.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_ll_adc.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_adc.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_adc_ex.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_flash.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_flash_ex.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_flash_ramfunc.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_dma_ex.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_pwr.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_pwr_ex.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_exti.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_i2s.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_i2s_ex.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_tim.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_tim_ex.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_pcd.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_pcd_ex.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_uart.c \
    $(ST_HAL_DRIVER)/Src/stm32$(ST_FAMILY)xx_hal_usart.c \
    
