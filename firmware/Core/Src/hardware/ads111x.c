/**
 * @file ads111x.c
 *
 * STM32 driver for ADS1113/ADS1114/ADS1115, ADS1013/ADS1014/ADS1015 I2C ADC
 *
 * Ported from esp-idf by Lucio Tarantino
 *
 */


#include "ads111x.h"
#include "utils.h"

#define REG_CONVERSION 0
#define REG_CONFIG     1
#define REG_THRESH_L   2
#define REG_THRESH_H   3

#define COMP_QUE_OFFSET  1
#define COMP_QUE_MASK    0x03
#define COMP_LAT_OFFSET  2
#define COMP_LAT_MASK    0x01
#define COMP_POL_OFFSET  3
#define COMP_POL_MASK    0x01
#define COMP_MODE_OFFSET 4
#define COMP_MODE_MASK   0x01
#define DR_OFFSET        5
#define DR_MASK          0x07
#define MODE_OFFSET      8
#define MODE_MASK        0x01
#define PGA_OFFSET       9
#define PGA_MASK         0x07
#define MUX_OFFSET       12
#define MUX_MASK         0x07
#define OS_OFFSET        15
#define OS_MASK          0x01

/* For STM32 */
#define CHECK(x) do { HAL_StatusTypeDef __; if ((__ = x) != HAL_OK) return __; } while (0)
#define CHECK_ARG(VAL) do { if (!(VAL)) return HAL_ERROR; } while (0)


const float ads111x_gain_values[] = {
	[ADS111X_GAIN_6V144]   = 6.144,
	[ADS111X_GAIN_4V096]   = 4.096,
	[ADS111X_GAIN_2V048]   = 2.048,
	[ADS111X_GAIN_1V024]   = 1.024,
	[ADS111X_GAIN_0V512]   = 0.512,
	[ADS111X_GAIN_0V256]   = 0.256,
	[ADS111X_GAIN_0V256_2] = 0.256,
	[ADS111X_GAIN_0V256_3] = 0.256
};

/**
 * Private functions
 */
/**
 * @brief Read a register from the ADS111X device over I2C.
 * @param hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *        the configuration information for the specified I2C.
 * @param reg The register address to read from.
 * @param val Pointer to store the read value.
 * @return HAL status.
 */
static HAL_StatusTypeDef read_reg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t *val);

/**
 * @brief Write to a register of the ADS111X device over I2C.
 * @param hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *        the configuration information for the specified I2C.
 * @param reg The register address to write to.
 * @param val The value to write to the register.
 * @return HAL status.
 */
static HAL_StatusTypeDef write_reg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t val);

/**
 * @brief Read a register from the ADS111X device over I2C.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure that contains
 *        the configuration information for the specified I2C.
 * @param reg The register address to read from.
 * @param val Pointer to store the read value.
 * @return HAL status indicating the result of the operation.
 */
static HAL_StatusTypeDef read_reg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t *val);

/**
 * @brief Write to a register of the ADS111X device over I2C.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure that contains
 *        the configuration information for the specified I2C.
 * @param reg The register address to write to.
 * @param val The value to write to the register.
 * @return HAL status indicating the result of the operation.
 */
static HAL_StatusTypeDef write_reg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t val);

/**
 * @brief Read specific bits from the configuration register.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param offs The bit offset to read from.
 * @param mask Bit mask to apply to the read value.
 * @param bits Pointer to store the read and masked bits.
 * @return HAL status indicating the result of the operation.
 */
static HAL_StatusTypeDef read_conf_bits(I2C_HandleTypeDef *hi2c, uint8_t offs, uint16_t mask, uint16_t *bits);

/**
 * @brief Write specific bits to the configuration register.
 * @param hi2c Pointer to an I2C_HandleTypeDef structure.
 * @param val The value to write.
 * @param offs The bit offset to write to.
 * @param mask Bit mask to apply to the value.
 * @return HAL status indicating the result of the operation.
 */
static HAL_StatusTypeDef write_conf_bits(I2C_HandleTypeDef *hi2c, uint16_t val, uint8_t offs, uint16_t mask);

/**
 * @brief read_reg for STM32
 * 
 */
static HAL_StatusTypeDef read_reg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t *val)
{
	uint8_t buf[2];
	HAL_StatusTypeDef res;
	if ((res = HAL_I2C_Mem_Read(hi2c, (uint16_t) (ADS111X_ADDR_GND << 1), reg, 1, buf, 2, 1000)) != HAL_OK)
	{
		LOG_ERROR("Could not read from register 0x%02x", reg);
		return res;
	}
	*val = (buf[0] << 8) | buf[1];

	return HAL_OK;
}

/**
 * @brief write_reg for STM32
 * 
 */
static HAL_StatusTypeDef write_reg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t val)
{
	uint8_t buf[2] = { val >> 8, val };
	HAL_StatusTypeDef res;
	if ((res = HAL_I2C_Mem_Write(hi2c, (uint16_t) (ADS111X_ADDR_GND << 1), reg, 1, buf, 2, 1000)) != HAL_OK)
	{
		LOG_ERROR("Could not write 0x%04x to register 0x%02x", val, reg);
		return res;
	}

	return HAL_OK;
}

/**
 * @brief read_conf_bits for STM32
 * 
 */
static HAL_StatusTypeDef read_conf_bits(I2C_HandleTypeDef *hi2c, uint8_t offs, uint16_t mask,
		uint16_t *bits)
{
	CHECK_ARG(hi2c);

	uint16_t val;

	CHECK(read_reg(hi2c, REG_CONFIG, &val));

	*bits = (val >> offs) & mask;

	return HAL_OK;
}

/**
 * @brief write_conf_bits for STM32
 * 
 */
static HAL_StatusTypeDef write_conf_bits(I2C_HandleTypeDef *hi2c, uint16_t val, uint8_t offs,
		uint16_t mask)
{
	CHECK_ARG(hi2c);

	uint16_t old;

	CHECK(read_reg(hi2c, REG_CONFIG, &old));
	CHECK(write_reg(hi2c, REG_CONFIG, (old & ~(mask << offs)) | (val << offs)));

	return HAL_OK;
}

#define READ_CONFIG(OFFS, MASK, VAR) do { \
		CHECK_ARG(VAR); \
		uint16_t bits; \
		CHECK(read_conf_bits(hi2c, OFFS, MASK, &bits)); \
		*VAR = bits; \
		return HAL_OK; \
	} while(0)


///////////////////////////////////////////////////////////////////////////////
/**
 * @brief Initialize device descriptor
 * 
 */
HAL_StatusTypeDef ads111x_init_desc(I2C_HandleTypeDef *hi2c, uint8_t addr)
{
	CHECK_ARG(hi2c);

	if (addr != ADS111X_ADDR_GND && addr != ADS111X_ADDR_VCC
			&& addr != ADS111X_ADDR_SDA && addr != ADS111X_ADDR_SCL)
	{
		LOG_ERROR("Invalid I2C address");
		return HAL_ERROR;
	}

	return HAL_OK;
}

/**
 * @brief Get device operational status
 * 
 */
HAL_StatusTypeDef ads111x_is_busy(I2C_HandleTypeDef *hi2c, uint8_t *busy)
{
	CHECK_ARG(hi2c && busy);

	uint16_t r;
	CHECK(read_conf_bits(hi2c, OS_OFFSET, OS_MASK, &r));
	*busy = !r;

	return HAL_OK;
}

/**
 * @brief Begin a single conversion
 * 
 */
HAL_StatusTypeDef ads111x_start_conversion(I2C_HandleTypeDef *hi2c)
{
	return write_conf_bits(hi2c, 1, OS_OFFSET, OS_MASK);
}

/**
 * @brief Read last conversion result
 * 
 */
HAL_StatusTypeDef ads111x_get_value(I2C_HandleTypeDef *hi2c, int16_t *value)
{
	CHECK_ARG(hi2c && value);

	CHECK(read_reg(hi2c, REG_CONVERSION, (uint16_t *)value));

	return HAL_OK;
}

/**
 * @brief Read last conversion result for ADS101x
 * 
 */
HAL_StatusTypeDef ads101x_get_value(I2C_HandleTypeDef *hi2c, int16_t *value)
{
	CHECK_ARG(hi2c && value);

	CHECK(read_reg(hi2c, REG_CONVERSION, (uint16_t *)value));

	*value = *value >> 4;
	if (*value > 0x07FF)
	{
		// negative number - extend the sign to 16th bit
		*value |= 0xF000;
	}
	return HAL_OK;
}

/**
 * @brief Read the programmable gain amplifier configuration
 * 
 */
HAL_StatusTypeDef ads111x_get_gain(I2C_HandleTypeDef *hi2c, ads111x_gain_t *gain)
{
	READ_CONFIG(PGA_OFFSET, PGA_MASK, gain);
}

/**
 * @brief Configure the programmable gain amplifier
 * 
 */
HAL_StatusTypeDef ads111x_set_gain(I2C_HandleTypeDef *hi2c, ads111x_gain_t gain)
{
	return write_conf_bits(hi2c, gain, PGA_OFFSET, PGA_MASK);
}

/**
 * @brief Read the input multiplexer configuration
 * 
 */
HAL_StatusTypeDef ads111x_get_input_mux(I2C_HandleTypeDef *hi2c, ads111x_mux_t *mux)
{
	READ_CONFIG(MUX_OFFSET, MUX_MASK, mux);
}

/**
 * @brief Configure the input multiplexer configuration
 * 
 */
HAL_StatusTypeDef ads111x_set_input_mux(I2C_HandleTypeDef *hi2c, ads111x_mux_t mux)
{
	return write_conf_bits(hi2c, mux, MUX_OFFSET, MUX_MASK);
}

/**
 * @brief Read the device operating mode
 * 
 */
HAL_StatusTypeDef ads111x_get_mode(I2C_HandleTypeDef *hi2c, ads111x_mode_t *mode)
{
	READ_CONFIG(MODE_OFFSET, MODE_MASK, mode);
}

/**
 * @brief Set the device operating mode
 * 
 */
HAL_StatusTypeDef ads111x_set_mode(I2C_HandleTypeDef *hi2c, ads111x_mode_t mode)
{
	return write_conf_bits(hi2c, mode, MODE_OFFSET, MODE_MASK);
}

/**
 * @brief Read the data rate
 * 
 */
HAL_StatusTypeDef ads111x_get_data_rate(I2C_HandleTypeDef *hi2c, ads111x_data_rate_t *rate)
{
	READ_CONFIG(DR_OFFSET, DR_MASK, rate);
}

/**
 * @brief Configure the data rate
 * 
 */
HAL_StatusTypeDef ads111x_set_data_rate(I2C_HandleTypeDef *hi2c, ads111x_data_rate_t rate)
{
	return write_conf_bits(hi2c, rate, DR_OFFSET, DR_MASK);
}

/**
 * @brief Get comparator mode
 * 
 */
HAL_StatusTypeDef ads111x_get_comp_mode(I2C_HandleTypeDef *hi2c, ads111x_comp_mode_t *mode)
{
	READ_CONFIG(COMP_MODE_OFFSET, COMP_MODE_MASK, mode);
}

/**
 * @brief Set comparator mode
 * 
 */
HAL_StatusTypeDef ads111x_set_comp_mode(I2C_HandleTypeDef *hi2c, ads111x_comp_mode_t mode)
{
	return write_conf_bits(hi2c, mode, COMP_MODE_OFFSET, COMP_MODE_MASK);
}

/**
 * @brief Get comparator polarity
 * 
 */
HAL_StatusTypeDef ads111x_get_comp_polarity(I2C_HandleTypeDef *hi2c, ads111x_comp_polarity_t *polarity)
{
	READ_CONFIG(COMP_POL_OFFSET, COMP_POL_MASK, polarity);
}

/**
 * @brief Set comparator polarity
 * 
 */
HAL_StatusTypeDef ads111x_set_comp_polarity(I2C_HandleTypeDef *hi2c, ads111x_comp_polarity_t polarity)
{
	return write_conf_bits(hi2c, polarity, COMP_POL_OFFSET, COMP_POL_MASK);
}

/**
 * @brief Get comparator latch
 * 
 */
HAL_StatusTypeDef ads111x_get_comp_latch(I2C_HandleTypeDef *hi2c, ads111x_comp_latch_t *latch)
{
	READ_CONFIG(COMP_LAT_OFFSET, COMP_LAT_MASK, latch);
}

/**
 * @brief Set comparator latch
 * 
 */
HAL_StatusTypeDef ads111x_set_comp_latch(I2C_HandleTypeDef *hi2c, ads111x_comp_latch_t latch)
{
	return write_conf_bits(hi2c, latch, COMP_LAT_OFFSET, COMP_LAT_MASK);
}

/**
 * @brief Get comparator queue
 * 
 */
HAL_StatusTypeDef ads111x_get_comp_queue(I2C_HandleTypeDef *hi2c, ads111x_comp_queue_t *queue)
{
	READ_CONFIG(COMP_QUE_OFFSET, COMP_QUE_MASK, queue);
}

/**
 * @brief Set comparator queue
 * 
 */
HAL_StatusTypeDef ads111x_set_comp_queue(I2C_HandleTypeDef *hi2c, ads111x_comp_queue_t queue)
{
	return write_conf_bits(hi2c, queue, COMP_QUE_OFFSET, COMP_QUE_MASK);
}

/**
 * @brief Get comparator low threshold
 * 
 */
HAL_StatusTypeDef ads111x_get_comp_low_thresh(I2C_HandleTypeDef *hi2c, int16_t *th)
{
	CHECK_ARG(hi2c && th);

	CHECK(read_reg(hi2c, REG_THRESH_L, (uint16_t *)th));

	return HAL_OK;
}

/**
 * @brief Set comparator low threshold
 * 
 */
HAL_StatusTypeDef ads111x_set_comp_low_thresh(I2C_HandleTypeDef *hi2c, int16_t th)
{
	CHECK_ARG(hi2c);

	CHECK(write_reg(hi2c, REG_THRESH_L, th));

	return HAL_OK;
}

/**
 * @brief Get comparator high threshold
 * 
 */
HAL_StatusTypeDef ads111x_get_comp_high_thresh(I2C_HandleTypeDef *hi2c, int16_t *th)
{
	CHECK_ARG(hi2c && th);

	CHECK(read_reg(hi2c, REG_THRESH_H, (uint16_t *)th));

	return HAL_OK;
}

/**
 * @brief Set comparator high threshold
 * 
 */
HAL_StatusTypeDef ads111x_set_comp_high_thresh(I2C_HandleTypeDef *hi2c, int16_t th)
{
	CHECK_ARG(hi2c);

	CHECK(write_reg(hi2c, REG_THRESH_H, th));

	return HAL_OK;
}
