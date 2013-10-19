#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "crc.h"

/**
 * main
 */
int main (int argc, char *argv[])
{
	char *buf = NULL;
	int bufsize = 1048576;
	uint32_t crc_gen  = 0;
	uint32_t crc_read = 0;
	int fd = -1;
	size_t num = 0;

	if (argc != 2) {
		printf ("command line\n");
		exit (1);
	}

	buf = malloc (bufsize);
	if (!buf) {
		printf ("malloc\n");
		exit (1);
	}

	fd = open (argv[1], O_RDONLY);
	if (fd < 0) {
		printf ("open\n");
		exit (1);
	}

	num = read(fd, buf, bufsize);
	if (num < bufsize) {
		printf ("read\n");
		exit (1);
	}

	crc_read = *(uint32_t *) (buf+528);
	crc_gen = calc_crc (INITIAL_CRC, buf+532, 492);
	printf ("crc:\n\tread = 0x%08x\n\tgen  = 0x%08x\n\n", crc_read, crc_gen);

	crc_read = *(uint32_t *) (buf+4096);
	crc_gen = calc_crc (INITIAL_CRC, buf+4100, 508);
	printf ("crc:\n\tread = 0x%08x\n\tgen  = 0x%08x\n\n", crc_read, crc_gen);

	free (buf);
	return 0;
}

