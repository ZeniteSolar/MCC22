/**
 * @file ads111x.h
 * @{
 *
 * STM32 driver for ADS1113/ADS1114/ADS1115, ADS1013/ADS1014/ADS1015 I2C ADC
 *
 *
 * Ported from esp-idf by Lucio Tarantino
 */

#ifndef ADS111X_H
#define ADS111X_H

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_i2c.h"

#define ADS111X_ADDR_GND 0x48 //!< I2C device address with ADDR pin connected to ground
#define ADS111X_ADDR_VCC 0x49 //!< I2C device address with ADDR pin connected to VCC
#define ADS111X_ADDR_SDA 0x4a //!< I2C device address with ADDR pin connected to SDA
#define ADS111X_ADDR_SCL 0x4b //!< I2C device address with ADDR pin connected to SCL

#define ADS111X_MAX_VALUE 0x7fff //!< Maximum ADC value
#define ADS101X_MAX_VALUE 0x7ff

/**
 * @brief Gain amplifier
 */
typedef enum
{
	ADS111X_GAIN_6V144 = 0, //!< +-6.144V
	ADS111X_GAIN_4V096,     //!< +-4.096V
	ADS111X_GAIN_2V048,     //!< +-2.048V (default)
	ADS111X_GAIN_1V024,     //!< +-1.024V
	ADS111X_GAIN_0V512,     //!< +-0.512V
	ADS111X_GAIN_0V256,     //!< +-0.256V
	ADS111X_GAIN_0V256_2,   //!< +-0.256V (same as ADS111X_GAIN_0V256)
	ADS111X_GAIN_0V256_3,   //!< +-0.256V (same as ADS111X_GAIN_0V256)
} ads111x_gain_t;

/**
 * @brief Gain amplifier values
 */
extern const float ads111x_gain_values[];

/**
 * @brief Input multiplexer configuration (ADS1115 only)
 */
typedef enum
{
	ADS111X_MUX_0_1 = 0, //!< positive = AIN0, negative = AIN1 (default)
	ADS111X_MUX_0_3,     //!< positive = AIN0, negative = AIN3
	ADS111X_MUX_1_3,     //!< positive = AIN1, negative = AIN3
	ADS111X_MUX_2_3,     //!< positive = AIN2, negative = AIN3
	ADS111X_MUX_0_GND,   //!< positive = AIN0, negative = GND
	ADS111X_MUX_1_GND,   //!< positive = AIN1, negative = GND
	ADS111X_MUX_2_GND,   //!< positive = AIN2, negative = GND
	ADS111X_MUX_3_GND,   //!< positive = AIN3, negative = GND
} ads111x_mux_t;

/**
 * @brief Data rate
 */
typedef enum
{
	ADS111X_DATA_RATE_8 = 0, //!< 8 samples per second
	ADS111X_DATA_RATE_16,    //!< 16 samples per second
	ADS111X_DATA_RATE_32,    //!< 32 samples per second
	ADS111X_DATA_RATE_64,    //!< 64 samples per second
	ADS111X_DATA_RATE_128,   //!< 128 samples per second (default)
	ADS111X_DATA_RATE_250,   //!< 250 samples per second
	ADS111X_DATA_RATE_475,   //!< 475 samples per second
	ADS111X_DATA_RATE_860    //!< 860 samples per second
} ads111x_data_rate_t;

/**
 * @brief Operational mode
 */
typedef enum
{
	ADS111X_MODE_CONTINUOUS = 0, //!< Continuous conversion mode
	ADS111X_MODE_SINGLE_SHOT    //!< Power-down single-shot mode (default)
} ads111x_mode_t;

/**
 * @brief Comparator mode (ADS1114 and ADS1115 only)
 */
typedef enum
{
	ADS111X_COMP_MODE_NORMAL = 0, //!< Traditional comparator with hysteresis (default)
	ADS111X_COMP_MODE_WINDOW      //!< Window comparator
} ads111x_comp_mode_t;

/**
 * @brief Comparator polarity (ADS1114 and ADS1115 only)
 */
typedef enum
{
	ADS111X_COMP_POLARITY_LOW = 0, //!< Active low (default)
	ADS111X_COMP_POLARITY_HIGH     //!< Active high
} ads111x_comp_polarity_t;

/**
 * @brief Comparator latch (ADS1114 and ADS1115 only)
 */
typedef enum
{
	ADS111X_COMP_LATCH_DISABLED = 0, //!< Non-latching comparator (default)
	ADS111X_COMP_LATCH_ENABLED       //!< Latching comparator
} ads111x_comp_latch_t;

/**
 * @brief Comparator queue
 */
typedef enum
{
	ADS111X_COMP_QUEUE_1 = 0,   //!< Assert ALERT/RDY pin after one conversion
	ADS111X_COMP_QUEUE_2,       //!< Assert ALERT/RDY pin after two conversions
	ADS111X_COMP_QUEUE_4,       //!< Assert ALERT/RDY pin after four conversions
	ADS111X_COMP_QUEUE_DISABLED //!< Disable comparator (default)
} ads111x_comp_queue_t;


/**
 * @brief Initialize the ADS111X device descriptor.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param addr I2C address of the ADS111X device.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_init_desc(I2C_HandleTypeDef *hi2c, uint8_t addr);

/**
 * @brief Check if the ADS111X device is busy.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param busy Pointer to store the busy status (0 = not busy, 1 = busy).
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_is_busy(I2C_HandleTypeDef *hi2c, uint8_t *busy);

/**
 * @brief Start a single conversion on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_start_conversion(I2C_HandleTypeDef *hi2c);

/**
 * @brief Read the last conversion result from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param value Pointer to store the conversion result.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_value(I2C_HandleTypeDef *hi2c, int16_t *value);

/**
 * @brief Read the last conversion result from the ADS101X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param value Pointer to store the conversion result.
 * @return HAL status indicating the result of the operation.
 *         This function adjusts the result for ADS101X's 12-bit resolution.
 */
HAL_StatusTypeDef ads101x_get_value(I2C_HandleTypeDef *hi2c, int16_t *value);

/**
 * @brief Read the current gain setting from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param gain Pointer to an ads111x_gain_t variable to store the gain setting.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_gain(I2C_HandleTypeDef *hi2c, ads111x_gain_t *gain);

/**
 * @brief Set the gain on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param gain The gain setting to be set (ads111x_gain_t).
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_set_gain(I2C_HandleTypeDef *hi2c, ads111x_gain_t gain);

/**
 * @brief Read the current input multiplexer configuration from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param mux Pointer to an ads111x_mux_t variable to store the mux configuration.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_input_mux(I2C_HandleTypeDef *hi2c, ads111x_mux_t *mux);

/**
 * @brief Set the input multiplexer configuration on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param mux The multiplexer setting to be set (ads111x_mux_t).
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_set_input_mux(I2C_HandleTypeDef *hi2c, ads111x_mux_t mux);

/**
 * @brief Read the current operating mode from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param mode Pointer to an ads111x_mode_t variable to store the mode setting.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_mode(I2C_HandleTypeDef *hi2c, ads111x_mode_t *mode);

/**
 * @brief Set the operating mode on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param mode The operating mode to be set (ads111x_mode_t).
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_set_mode(I2C_HandleTypeDef *hi2c, ads111x_mode_t mode);

/**
 * @brief Read the current data rate from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param rate Pointer to an ads111x_data_rate_t variable to store the data rate setting.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_data_rate(I2C_HandleTypeDef *hi2c, ads111x_data_rate_t *rate);

/**
 * @brief Set the data rate on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param rate The data rate to be set (ads111x_data_rate_t).
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_set_data_rate(I2C_HandleTypeDef *hi2c, ads111x_data_rate_t rate);

/**
 * @brief Read the comparator mode from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param mode Pointer to an ads111x_comp_mode_t variable to store the comparator mode.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_comp_mode(I2C_HandleTypeDef *hi2c, ads111x_comp_mode_t *mode);

/**
 * @brief Set the comparator mode on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param mode The comparator mode to be set (ads111x_comp_mode_t).
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_set_comp_mode(I2C_HandleTypeDef *hi2c, ads111x_comp_mode_t mode);

/**
 * @brief Read the comparator polarity setting from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param polarity Pointer to an ads111x_comp_polarity_t variable to store the polarity setting.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_comp_polarity(I2C_HandleTypeDef *hi2c, ads111x_comp_polarity_t *polarity);

/**
 * @brief Set the comparator polarity on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param polarity The comparator polarity to be set (ads111x_comp_polarity_t).
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_set_comp_polarity(I2C_HandleTypeDef *hi2c, ads111x_comp_polarity_t polarity);

/**
 * @brief Read the comparator latch setting from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param latch Pointer to an ads111x_comp_latch_t variable to store the latch setting.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_comp_latch(I2C_HandleTypeDef *hi2c, ads111x_comp_latch_t *latch);

/**
 * @brief Set the comparator latch on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param latch The comparator latch to be set (ads111x_comp_latch_t).
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_set_comp_latch(I2C_HandleTypeDef *hi2c, ads111x_comp_latch_t latch);

/**
 * @brief Read the comparator queue setting from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param queue Pointer to an ads111x_comp_queue_t variable to store the queue setting.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_comp_queue(I2C_HandleTypeDef *hi2c, ads111x_comp_queue_t *queue);

/**
 * @brief Set the comparator queue on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param queue The comparator queue to be set (ads111x_comp_queue_t).
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_set_comp_queue(I2C_HandleTypeDef *hi2c, ads111x_comp_queue_t queue);

/**
 * @brief Read the low threshold value for the comparator from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param th Pointer to an integer to store the low threshold value.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_comp_low_thresh(I2C_HandleTypeDef *hi2c, int16_t *th);

/**
 * @brief Set the low threshold value for the comparator on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param th The low threshold value to be set.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_set_comp_low_thresh(I2C_HandleTypeDef *hi2c, int16_t th);

/**
 * @brief Read the high threshold value for the comparator from the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param th Pointer to an integer to store the high threshold value.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_get_comp_high_thresh(I2C_HandleTypeDef *hi2c, int16_t *th);

/**
 * @brief Set the high threshold value for the comparator on the ADS111X device.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param th The high threshold value to be set.
 * @return HAL status indicating the result of the operation.
 */
HAL_StatusTypeDef ads111x_set_comp_high_thresh(I2C_HandleTypeDef *hi2c, int16_t th);


#endif /* __ADS111X_H__ */