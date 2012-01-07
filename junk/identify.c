// gcc -Wall -g -o identify{,.c}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <endian.h>

/**
 * identify_btrfs
 */
int identify_btrfs (char *name, char *buffer, int bufsize)
{
	if (strncmp (buffer+65600, "_BHRfS_M", 8))
		return 0;

	printf ("%s: btrfs\n", name);

	if (buffer[65835])
		printf ("\tlabel = %s\n", buffer+65835);
	else
		printf ("\tno label\n");

	printf ("\tsector size %d\n", *(int*) (buffer+65680));
	printf ("\tnode   size %d\n", *(int*) (buffer+65684));
	printf ("\tleaf   size %d\n", *(int*) (buffer+65688));

	return 1;
}

/**
 * identify_ext2
 */
int identify_ext2 (char *name, char *buffer, int bufsize)
{
	if (*(unsigned short int *) (buffer+1080) != 0xEF53)
		return 0;

	if ((buffer[1116] & 4) == 0)
		printf ("%s: ext2\n", name);
	else if (buffer[1124] < 8)
		printf ("%s: ext3\n", name);
	else
		printf ("%s: ext4\n", name);

	printf ("\tuuid = %02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n",
		buffer[1128], buffer[1129], buffer[1130], buffer[1131], buffer[1132], buffer[1133], buffer[1134], buffer[1135], buffer[1136], buffer[1137], buffer[1138], buffer[1139], buffer[1140], buffer[1141], buffer[1142], buffer[1143]);

	if (buffer[1144])
		printf ("\tlabel = %s\n", buffer+1144);
	else
		printf ("\tno label\n");

	return 1;
}

/**
 * identify_ntfs
 */
int identify_ntfs (char *name, char *buffer, int bufsize)
{
	if (strncmp (buffer+3, "NTFS    ", 8))
		return 0;

	printf ("%s: ntfs\n", name);
	printf ("\tserial number = 0x%llx\n", *(long long*) (buffer+72));

	return 1;
}

/**
 * identify_reiserfs
 */
int identify_reiserfs (char *name, char *buffer, int bufsize)
{
	if (strncmp (buffer+65588, "ReIsErFs",  8) &&
	    strncmp (buffer+65588, "ReIsEr2Fs", 8) &&
	    strncmp (buffer+65588, "ReIsEr3Fs", 9)) {
		return 0;
	}

	printf ("%s: reiserfs\n", name);
	if (buffer[65636])
		printf ("\tlabel = %.16s\n", buffer+65636);
	else
		printf ("\tno label\n");

	printf ("\tblock size %d\n", *(short int*) (buffer+65580));
	printf ("\tnum blocks %d\n", *(int*) (buffer+65536));

	return 1;
}

/**
 * identify_swap
 */
int identify_swap (char *name, char *buffer, int bufsize)
{
	if (strncmp (buffer+4086, "SWAPSPACE2",           10) &&
	    strncmp (buffer+4086, "SWAP-SPACE",           10) &&
	    strncmp (buffer+4076, "SWAPSPACE2S1SUSPEND",  19) &&
	    strncmp (buffer+4076, "SWAPSPACE2LINHIB0001", 20)) {
		return 0;
	}

	printf ("%s: swap space\n", name);
	printf ("\tversion %d (4K pages)\n", *(int*) (buffer + 1024));		// 4 bytes
	printf ("\tsize %d pages\n", *(int*) (buffer + 1028));			// 4 bytes
	printf ("\tuuid = %02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n",
		buffer[1036], buffer[1037], buffer[1038], buffer[1039], buffer[1040], buffer[1041], buffer[1042], buffer[1043], buffer[1044], buffer[1045], buffer[1046], buffer[1047], buffer[1048], buffer[1049], buffer[1050], buffer[1051]);

	if (buffer[1052])
		printf ("\tlabel = %.16s\n", buffer+1052);
	else
		printf ("\tno label\n");

	return 1;
}

/**
 * identify_vfat
 */
int identify_vfat (char *name, char *buffer, int bufsize)
{
	if (*(unsigned short int *) (buffer+510) != 0xAA55)	// boot signature
		return 0;
	if (buffer[3] == 0)					// system name
		return 0;
	if (*(unsigned short int *) (buffer+11) == 0)		// sectors/cluster
		return 0;
	if (buffer[13] == 0)					// cluster size
		return 0;
	if (*(unsigned short int *) (buffer+14) == 0)		// reserved
		return 0;
	if ((buffer[16] < 1) || (buffer[16] > 4))		// number of FATs
		return 0;

	printf ("%s: vfat\n", name);
	printf ("\tserial number = 0x%lx\n", *(long*) (buffer+67));
	if (buffer[71])
		printf ("\tlabel = %.11s\n", buffer+71);

	return 1;
}

/**
 * identify_xfs
 */
int identify_xfs (char *name, char *buffer, int bufsize)
{
	if (strncmp (buffer, "XFSB", 4))
		return 0;

	printf ("%s: xfs\n", name);

	printf ("\tblock size %d\n", be32toh (*(long*) (buffer+4)));
	printf ("\tinode size %d\n", be16toh (*(short int*) (buffer+104)));

	return 1;
}

/**
 * identify_lvm
 */
int identify_lvm (char *name, char *buffer, int bufsize)
{
	if (strncmp (buffer+536, "LVM2 001", 8))
		return 0;

	printf ("%s: lvm\n", name);

	printf ("\tuuid = %.6s-%.4s-%.4s-%.4s-%.4s-%.4s-%.6s\n",
		buffer+544, buffer+550, buffer+554, buffer+558, buffer+562, buffer+566, buffer+570);
	printf ("\tsize = %lld\n", *(long long*) (buffer+576));

	return 1;
}


/**
 * identify
 */
int identify (char *name, char *buffer, int bufsize)
{
	return (identify_btrfs    (name, buffer, bufsize) ||
		identify_ext2     (name, buffer, bufsize) ||
		identify_ntfs     (name, buffer, bufsize) ||
		identify_reiserfs (name, buffer, bufsize) ||
		identify_swap     (name, buffer, bufsize) ||
		identify_vfat     (name, buffer, bufsize) ||
		identify_xfs      (name, buffer, bufsize) ||
		identify_lvm      (name, buffer, bufsize));
}

/**
 * main
 */
int main (int argc, char *argv[])
{
	int fd = -1;
	char *buffer = NULL;
	int bufsize = 65*1024;
	int count = 0;
	int i;

	if (argc < 2) {
		printf ("Usage: identify disk\n");
		return 1;
	}

	buffer = malloc (bufsize);
	if (!buffer) {
		printf ("can't allocate buffer\n");
	}

	for (i = 1; i < argc; i++) {
		fd = open (argv[i], O_RDONLY);
		if (fd < 0) {
			printf ("can't open disk: %s\n", argv[1]);
			continue;
		}

		count = read (fd, buffer, bufsize);
		if (count < bufsize) {
			printf ("couldn't read %d bytes from %s\n", bufsize, argv[1]);
			close (fd);
			continue;
		}

		if (!identify (argv[i], buffer, bufsize))
			printf ("%s: NO IDEA\n", argv[i]);
	}

	free (buffer);
	return 0;
}

