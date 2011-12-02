#include <stdio.h>
#include <parted/parted.h>

int main (int argc, char *argv[])
{
	PedGeometry geom;
	PedFileSystemType *fs = NULL;
	int i;

	for (i = 1; i < argc; i++) {

		geom.dev = ped_device_get (argv[i]);
		geom.start = 0;
		geom.length = 2097152;
		geom.end = 2097152;

		fs = ped_file_system_probe (&geom);

		if (fs) {
			printf ("fs = %s\n", fs->name);
		}
	}

	return 0;
}

