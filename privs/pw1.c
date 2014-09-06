#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>

int
main (int argc, char *argv[])
{
	struct passwd *pw;

	if (argc < 2) {
		printf ("Usage: %s <user name>\n", argv[0]);
		return 1;
	}

	pw = getpwnam (argv[1]);
	if (pw == NULL) {
		printf ("Failed to find user: %s\n", argv[1]);
		return 1;
	}

	printf ("uid: %d, gid: %d, home dir: %s\n",
		pw->pw_uid,
		pw->pw_gid,
		pw->pw_dir);
	return 0;
}
