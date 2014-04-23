#include <cstdint>
#include <iostream>
#include <vector>
#include <fstream>

#include <fcntl.h>
#include <unistd.h>

#include "crc.h"
#include "../endian.h"

/**
 * main
 */
int main (int argc, char *argv[])
{
	const int bufsize = 8192;
	std::vector<std::uint8_t> buffer;
	buffer.resize (bufsize);

	if (argc != 2) {
		std::cout << "command line\n" << std::endl;
		exit (1);
	}

	std::ifstream file (argv[1], std::ifstream::binary);
	if (!file) {
		std::cout << "open\n" << std::endl;
		exit (1);
	}

	file.read ((char*) buffer.data(), bufsize);
	file.close();

	uint32_t crc_read = le32_to_cpup (buffer.data()+528);
	uint32_t crc_gen  = calc_crc     (buffer.data()+532, 492);

	std::cout << "crc:" << std::endl;
	std::cout << "\tread = 0x" << std::hex << crc_read << std::endl;
	std::cout << "\tgen  = 0x" << std::hex << crc_gen  << std::endl;

	crc_read = le32_to_cpup (buffer.data()+4096);
	crc_gen  = calc_crc     (buffer.data()+4100, 508);

	std::cout << "crc:" << std::endl;
	std::cout << "\tread = 0x" << std::hex << crc_read << std::endl;
	std::cout << "\tgen  = 0x" << std::hex << crc_gen  << std::endl;

	return 0;
}

