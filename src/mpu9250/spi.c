/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev0.1";
static uint32_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;
static int verbose;


int fd;

uint8_t default_tx[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0x0D,
};

uint8_t default_rx[ARRAY_SIZE(default_tx)] = {0, };
char *input_tx;


static void transfer(uint8_t const *tx, uint8_t const *rx, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");


    // printf("rx is %s\n", rx);
	// if (verbose)
	// 	hex_dump(tx, len, 32, "TX");
	// hex_dump(rx, len, 32, "RX");
}

static void print_usage(const char *prog)
{
	printf("Usage: %s [-DsbdlHOLC3]\n", prog);
	puts("  -D --device   device to use (default /dev/spidev1.1)\n"
	     "  -s --speed    max speed (Hz)\n"
	     "  -d --delay    delay (usec)\n"
	     "  -b --bpw      bits per word \n"
	     "  -l --loop     loopback\n"
	     "  -H --cpha     clock phase\n"
	     "  -O --cpol     clock polarity\n"
	     "  -L --lsb      least significant bit first\n"
	     "  -C --cs-high  chip select active high\n"
	     "  -3 --3wire    SI/SO signals shared\n"
	     "  -v --verbose  Verbose (show tx buffer)\n"
	     "  -p            Send data (e.g. \"1234\\xde\\xad\")\n"
	     "  -N --no-cs    no chip select\n"
	     "  -R --ready    slave pulls low to pause\n"
	     "  -2 --dual     dual transfer\n"
	     "  -4 --quad     quad transfer\n");
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "device",  1, 0, 'D' },
			{ "speed",   1, 0, 's' },
			{ "delay",   1, 0, 'd' },
			{ "bpw",     1, 0, 'b' },
			{ "loop",    0, 0, 'l' },
			{ "cpha",    0, 0, 'H' },
			{ "cpol",    0, 0, 'O' },
			{ "lsb",     0, 0, 'L' },
			{ "cs-high", 0, 0, 'C' },
			{ "3wire",   0, 0, '3' },
			{ "no-cs",   0, 0, 'N' },
			{ "ready",   0, 0, 'R' },
			{ "dual",    0, 0, '2' },
			{ "verbose", 0, 0, 'v' },
			{ "quad",    0, 0, '4' },
			{ NULL, 0, 0, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR24p:v", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'D':
			device = optarg;
			break;
		case 's':
			speed = atoi(optarg);
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		case 'b':
			bits = atoi(optarg);
			break;
		case 'l':
			mode |= SPI_LOOP;
			break;
		case 'H':
			mode |= SPI_CPHA;
			break;
		case 'O':
			mode |= SPI_CPOL;
			break;
		case 'L':
			mode |= SPI_LSB_FIRST;
			break;
		case 'C':
			mode |= SPI_CS_HIGH;
			break;
		case '3':
			mode |= SPI_3WIRE;
			break;
		case 'N':
			mode |= SPI_NO_CS;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'R':
			mode |= SPI_READY;
			break;
		case 'p':
			input_tx = optarg;
			break;
		case '2':
			mode |= SPI_TX_DUAL;
			break;
		case '4':
			mode |= SPI_TX_QUAD;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
	if (mode & SPI_LOOP) {
		if (mode & SPI_TX_DUAL)
			mode |= SPI_RX_DUAL;
		if (mode & SPI_TX_QUAD)
			mode |= SPI_RX_QUAD;
	}
}

int init_spi(int argc, char *argv[])
{
	int ret = 0;

	parse_opts(argc, argv);

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	return ret;
}




uint8_t spi_transfer_byte(uint8_t out)
{
    uint8_t in;
    transfer(&out, &in, 1);
    return in;
}


uint8_t spi_transfer_reg(uint8_t reg, uint8_t out)
{
	uint8_t dummy_rx[2], dummy_tx[2];

	dummy_tx[0] = reg;
	dummy_tx[1] = out;
    transfer(dummy_tx, dummy_rx, 2);
    // printf("receive val 0x%02x\n", dummy_rx[1]);
    return dummy_rx[1];
}


void spi_transfer_regs(uint8_t reg, const void *out, void *in, size_t len)
{
	size_t buf_len = len + 1;

	uint8_t dummy_rx[buf_len];
	uint8_t dummy_tx[buf_len];
    // transfer(&reg, NULL, 1);
    // transfer(&reg, &dummy_rx, 1);
    // usleep(2);
	dummy_tx[0] = reg;
	if (out != NULL)
		memcpy(&dummy_tx[1], out, len);
	else
		memset(&dummy_tx[1], 0, len);

	transfer(dummy_tx, dummy_rx, buf_len);
	memcpy(in, &dummy_rx[1], len);

	// printf("reg is 0x%02x\n", reg);
}




