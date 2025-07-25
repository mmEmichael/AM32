/*
 * ADC.c
 *
 *  Created on: May 20, 2020
 *      Author: Alka
 */
#include "ADC.h"

#ifdef USE_ADC_INPUT
uint16_t ADCDataDMA[4];
#else
uint16_t ADCDataDMA[3];
#endif

extern uint16_t ADC_raw_temp;
extern uint16_t ADC_raw_volts;
extern uint16_t ADC_raw_current;
extern uint16_t ADC_raw_input;

void ADC_DMA_Callback()
{ // read dma buffer and set extern variables

#ifdef USE_ADC_INPUT
    ADC_raw_temp = ADCDataDMA[3];
    ADC_raw_volts = ADCDataDMA[1] / 2;
    ADC_raw_current = ADCDataDMA[2];
    ADC_raw_input = ADCDataDMA[0];

#else
    ADC_raw_temp = ADCDataDMA[2];
    if (VOLTAGE_ADC_PIN > CURRENT_ADC_PIN) {
        ADC_raw_volts = ADCDataDMA[1];
        ADC_raw_current = ADCDataDMA[0];
    } else {
        ADC_raw_volts = ADCDataDMA[0];
        ADC_raw_current = ADCDataDMA[1];
    }
#endif
}

void enableADC_DMA()
{ // enables channel

    //	NVIC_SetPriority(DMA1_Channel1_IRQn, 3);
    //	NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    LL_DMA_ConfigAddresses(
        DMA1, LL_DMA_CHANNEL_1,
        LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
        (uint32_t)&ADCDataDMA, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    /* Set DMA transfer size */
#ifdef USE_ADC_INPUT
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 4);
#else
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 3);

#endif
    // LL_DMA_EnableIT_TC(DMA1,LL_DMA_CHANNEL_1);

    // LL_DMA_EnableIT_TE(DMA1,LL_DMA_CHANNEL_1);

    /*## Activation of DMA
     * #####################################################*/
    /* Enable the DMA transfer */
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
}

void activateADC()
{ // called right after enable regular conversions are
    // started by software and DMA interrupt happens at end of
    // transfer

    __IO uint32_t wait_loop_index = 0;

    LL_ADC_StartCalibration(ADC1);

    /* Poll for ADC effectively calibrated */

    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0) {
    }
    wait_loop_index = (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES >> 1);
    while (wait_loop_index != 0) {
        wait_loop_index--;
    }
    LL_ADC_Enable(ADC1);
    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0) {
    }
}

// void ADC_Init(void)
// {
//     LL_ADC_InitTypeDef ADC_InitStruct = { 0 };
//     LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = { 0 };

//     LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

//     /* Peripheral clock enable */
//     LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);

//     LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
//     /**ADC GPIO Configuration
//     PA3   ------> ADC_IN3
//     PA6   ------> ADC_IN6
//     */
// #ifdef USE_ADC_INPUT
//     GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
//     GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
//     GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//     LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
// #endif

//     GPIO_InitStruct.Pin = CURRENT_ADC_PIN;
//     GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
//     GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//     LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     GPIO_InitStruct.Pin = VOLTAGE_ADC_PIN;
//     GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
//     GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//     LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1,
//         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

//     LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

//     LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);

//     LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

//     LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

//     LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

//     LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

// #ifdef USE_ADC_INPUT
//     LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_2);
// #endif

//     LL_ADC_REG_SetSequencerChAdd(ADC1, VOLTAGE_ADC_CHANNEL);
//     /** Configure Regular Channel
//      */
//     LL_ADC_REG_SetSequencerChAdd(ADC1, CURRENT_ADC_CHANNEL);
//     /** Configure Regular Channel
//      */
//     LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_TEMPSENSOR);
//     /** Configure Internal Channel
//      */
//     LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),
//         LL_ADC_PATH_INTERNAL_TEMPSENSOR);
//     /** Configure the global features of the ADC (Clock, Resolution, Data
//      * Alignment and number of conversion)
//      */
//     ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
//     ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
//     ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
//     ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
//     LL_ADC_Init(ADC1, &ADC_InitStruct);
//     ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
//     ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
//     ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
//     ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_LIMITED;
//     ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
//     LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
//     // LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_FALLING);
//     LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
//     LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_71CYCLES_5);
//     LL_ADC_DisableIT_EOC(ADC1);
//     LL_ADC_DisableIT_EOS(ADC1);
// }

void ADC_Init(void)
{
    LL_ADC_InitTypeDef ADC_InitStruct = {0};
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 1. Включение тактирования */
    // Для APM32F051 используем прямое обращение к регистру RCC:
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;  // Включение тактирования ADC1
    
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    /* 2. Настройка GPIO (остаётся без изменений) */
    GPIO_InitStruct.Pin = CURRENT_ADC_PIN | VOLTAGE_ADC_PIN;
    #ifdef USE_ADC_INPUT
    GPIO_InitStruct.Pin |= LL_GPIO_PIN_2;
    #endif
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 3. Настройка ADC */
    ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADC1, &ADC_InitStruct);

    /* 4. Настройка каналов */
    #ifdef USE_ADC_INPUT
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_2);
    #endif
    LL_ADC_REG_SetSequencerChAdd(ADC1, VOLTAGE_ADC_CHANNEL);
    LL_ADC_REG_SetSequencerChAdd(ADC1, CURRENT_ADC_CHANNEL);
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_TEMPSENSOR);

    /* 5. Настройка регулярных преобразований */
    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
    ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);

    /* 6. Настройка DMA */
    LL_DMA_DeInit(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_MEDIUM);
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA));
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&ADCDataDMA);
    #ifdef USE_ADC_INPUT
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 4);
    #else
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 3);
    #endif
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

    /* 7. Настройка внутреннего канала */
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), 
                                 LL_ADC_PATH_INTERNAL_TEMPSENSOR);
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_71CYCLES_5);

    /* 8. Отключение ненужных прерываний */
    LL_ADC_DisableIT_EOC(ADC1);
    LL_ADC_DisableIT_EOS(ADC1);
}
