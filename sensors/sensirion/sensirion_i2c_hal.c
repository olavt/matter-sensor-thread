#include "../../sensors/sensirion/sensirion_i2c_hal.h"

#include "sl_i2cspm_instances.h"
#include "ustimer.h"
#include "../../sensors/sensirion/sensirion_common.h"
#include "../../sensors/sensirion/sensirion_config.h"

/**
 * Select the current i2c bus by index.
 * All following i2c operations will be directed at that bus.
 *
 * THE IMPLEMENTATION IS OPTIONAL ON SINGLE-BUS SETUPS (all sensors on the same
 * bus)
 *
 * @param bus_idx   Bus index to select
 * @returns         0 on success, an error code otherwise
 */
int16_t sensirion_i2c_hal_select_bus(uint8_t bus_idx) {
    /* TODO:IMPLEMENT or leave empty if all sensors are located on one single
     * bus
     */
    return NOT_IMPLEMENTED_ERROR;
}

/**
 * Initialize all hard- and software components that are needed for the I2C
 * communication.
 */
void sensirion_i2c_hal_init(void) {
  USTIMER_Init();
}

/**
 * Release all resources initialized by sensirion_i2c_hal_init().
 */
void sensirion_i2c_hal_free(void) {
}

/**
 * Execute one read transaction on the I2C bus, reading a given number of bytes.
 * If the device does not acknowledge the read command, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to read from
 * @param data    pointer to the buffer where the data is to be stored
 * @param count   number of bytes to read from I2C and store in the buffer
 * @returns 0 on success, error code otherwise
 */
int8_t sensirion_i2c_hal_read(uint8_t address, uint8_t* data, uint8_t count) {
    I2C_TransferSeq_TypeDef    seq;
    I2C_TransferReturn_TypeDef ret;

    // 7 bit address - Use format AAAA AAAX
    seq.addr  = address << 1;

    seq.flags = I2C_FLAG_READ;
    seq.buf[0].data   = data;
    seq.buf[0].len    = count;

    seq.buf[1].data = 0;
    seq.buf[1].len  = 0;

    ret = I2CSPM_Transfer(sl_i2cspm_sensor, &seq);

    return ret;
}

/**
 * Execute one write transaction on the I2C bus, sending a given number of
 * bytes. The bytes in the supplied buffer must be sent to the given address. If
 * the slave device does not acknowledge any of the bytes, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to write to
 * @param data    pointer to the buffer containing the data to write
 * @param count   number of bytes to read from the buffer and send over I2C
 * @returns 0 on success, error code otherwise
 */
int8_t sensirion_i2c_hal_write(uint8_t address, const uint8_t* data,
                               uint8_t count) {
    I2C_TransferSeq_TypeDef    seq;
    I2C_TransferReturn_TypeDef ret;

    // 7 bit address - Use format AAAA AAAX
    seq.addr  = address << 1;

    seq.flags = I2C_FLAG_WRITE;

    seq.buf[0].data   = (uint8_t*)data; // Safe to cast, since the write operation only reads the data buffer
    seq.buf[0].len    = count;

    seq.buf[1].data = 0;
    seq.buf[1].len  = 0;

    ret = I2CSPM_Transfer(sl_i2cspm_sensor, &seq);

    return ret;
}

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution for at least the given time, but may also sleep longer.
 *
 * Despite the unit, a <10 millisecond precision is sufficient.
 *
 * @param useconds the sleep time in microseconds
 */
void sensirion_i2c_hal_sleep_usec(uint32_t useconds) {
  USTIMER_Delay(useconds);
}
