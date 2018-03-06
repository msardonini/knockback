#ifndef SPI_H
#define SPI_H


int init_spi(int argc, char *argv[]);




/**
 * @brief Transfer one byte on the given SPI bus
 * @param[in] out       byte to send out, set NULL if only receiving
 *
 * @return              the received byte
 */
uint8_t spi_transfer_byte(uint8_t out);

/**
 * @brief   Transfer a number bytes using the given SPI bus
 *
 * @param[in]  out      buffer to send data from, set NULL if only receiving
 * @param[out] in       buffer to read into, set NULL if only sending
 * @param[in]  len      number of bytes to transfer
 */
void spi_transfer_bytes(const void *out, void *in, size_t len);

/**
 * @brief   Transfer one byte to/from a given register address
 *
 * This function is a shortcut function for easier handling of SPI devices that
 * implement a register based access scheme.
 *
 * @param[in] reg       register address to transfer data to/from
 * @param[in] out       byte to send, set NULL if only receiving data
 *
 * @return              value that was read from the given register address
 */
uint8_t spi_transfer_reg(uint8_t reg, uint8_t out);

/**
 * @brief   Transfer a number of bytes to/from a given register address
 *
 * This function is a shortcut function for easier handling of SPI devices that
 * implement a register based access scheme.
 *
 * @param[in]  reg      register address to transfer data to/from
 * @param[in]  out      buffer to send data from, set NULL if only receiving
 * @param[out] in       buffer to read into, set NULL if only sending
 * @param[in]  len      number of bytes to transfer
 */
void spi_transfer_regs(uint8_t reg, const void *out, uint8_t *in, size_t len);




#endif //SPI_H