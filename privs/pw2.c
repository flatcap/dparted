#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int
main()
{
	char *username = "flatcap";

	struct passwd *pwd = calloc (1, sizeof (struct passwd));
	if (!pwd)
		return 1;

	size_t buffer_len = sysconf (_SC_GETPW_R_SIZE_MAX) * sizeof (char);
	char *buffer = malloc (buffer_len);
	if (!buffer)
		return 1;

	getpwnam_r (username, pwd, buffer, buffer_len, &pwd);
	if (!pwd)
		return 1;

	printf ("uid: %d\n", pwd->pw_uid);
	printf ("gid: %d\n", pwd->pw_gid);

	return 0;
}

