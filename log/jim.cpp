#include <iostream>
#include <map>
#include <functional>
#include <typeinfo>
#include <vector>
#include <cstring>

#include "../severity.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

#if 0
// Default handlers
extern log_callback_t log_stdout;
extern log_callback_t log_stderr;
extern log_callback_t log_syslog;
extern log_callback_t log_journal;
log_callback_t log_open_file (const std::string& filename, bool truncate);
#endif

typedef std::function<int(Severity level, const char* function, const char* file, int line, const char* message)> log_callback_t;

std::multimap<Severity,log_callback_t> log_mux;

int log_stdout (Severity UNUSED(level), const char* UNUSED(function), const char* UNUSED(file), int UNUSED(line), const char* message)
{
	std::cout << __FUNCTION__ << ":" << message << std::endl;
	return 0;
}

int log_stderr (Severity UNUSED(level), const char* UNUSED(function), const char* UNUSED(file), int UNUSED(line), const char* message)
{
	std::cout << __FUNCTION__ << ":" << message << std::endl;
	return 0;
}


void log_init (Severity s, log_callback_t cb)
{
	log_mux.insert (std::make_pair(s,cb));
}

int log (Severity sev, const char* message)
{
	for (auto i : log_mux) {
		if ((bool) (i.first & sev)) {
			i.second (sev, __PRETTY_FUNCTION__, __FILE__, __LINE__, message);
		}
	}

	return 0;
}

int
main()
{
	//log_callback_t mylog = log_open_file ("logfile.txt");

	log_init (Severity::Debug,  log_stdout);
	log_init (~Severity::Debug, log_stderr);

	log (Severity::Debug, "debug");
	log (Severity::Info,  "info");

	//log_close (my_log);
}

