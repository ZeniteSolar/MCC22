#include <stdio.h>
#include "utils.h"
#include "comparators.h"
#include "main.h"
#include "dac.h"

void comparator_init_dac(COMP_HandleTypeDef *hcomp, DAC_HandleTypeDef *hdac, uint32_t dac_channel, float voltage)
{
    // Start DAC and set the output voltage
    HAL_DAC_Start(hdac, dac_channel);
    dac_set_voltage(hdac, dac_channel, voltage);

    // Start compator
    HAL_COMP_Start(hcomp);
}

void comparator_init_extern(COMP_HandleTypeDef *hcomp, DAC_HandleTypeDef *hdac, uint32_t dac_channel)
{
    // Start compator
    HAL_COMP_Start(hcomp);
}

/**
 * @brief  Comparator trigger callback.
 * @param  hcomp  COMP handle
 */
void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
    if (hcomp->Instance == COMP1)
        HAL_GPIO_WritePin(DB_0_GPIO_Port, DB_0_Pin, GPIO_PIN_SET);
    if (hcomp->Instance == COMP2)
        HAL_GPIO_WritePin(DB_1_GPIO_Port, DB_1_Pin, GPIO_PIN_SET);
}