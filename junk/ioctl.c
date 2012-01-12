// gcc -Wall -g -o ioctl{,.c}

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <linux/fs.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/**
 * main
 */
int main (int argc, char *argv[])
{
	struct stat st;
	int res = -1;
	long long file_size_in_bytes = 0;
	int fd = -1;

	if (argc != 2) {
		printf ("Usage: ioctl device\n");
		return 1;
	}

	res = stat (argv[1], &st);
	if (res < 0) {
		printf ("%s doesn't exist\n", argv[1]);
		return 1;
	}

	fd = open (argv[1], O_RDONLY);
	printf ("fd = %d\n", fd);

	res = ioctl (fd, BLKGETSIZE64, &file_size_in_bytes);
	printf ("res = %d\n", res);
	close (fd);

	printf ("name = %s\n", argv[1]);
	printf ("size (stat)  = %ld\n", st.st_size);
	printf ("size (ioctl) = %lld\n", file_size_in_bytes);

	return 0;
}
