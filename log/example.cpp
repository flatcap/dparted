
#include "log.h"	// global log object

int __log(const char *function, const char *file, int line, u32 level, const char *format, ...) __attribute__((format(printf, 5, 6)));

default handler

std::multimap<log_level,handler> redirect;	// many-to-many
	LOG_ERROR, stdout_handler
	LOG_CRIT,  journal
	LOG_INFO,  message_box

class LogOutputBase {};
class LogOutputTerminal : public LogOutputBase {
	bool reset;
	int colour;
};
class LogOutputJournal : public LogOutputBase { ... };

namespace Log
{
	LogCore main_object;

	LogStream critical;
	LogStream error;
	LogStream info;
	LogStream perror;
	LogStream progress;
	LogStream quiet;
	LogStream verbose;
	LogStream warning;
	LogStream debug;
}

void function1 (void)
{
	log (Log::log);		// copy global object

	Log::debug ("message 1\n");
	Log::info  ("message 2\n");

	if (condition) {
		Log::quell();		// stops logging for rest of function
	}

	Log::info  ("message 3\n");
	Log::debug ("message 4\n");
}

void function2 (void)
{
	LogTrace __le(__PRETTY_FUNCTION__, __FILE__, __LINE__);

	Log::debug ("message 1\n");
	Log::info  ("message 2\n");

	if (condition) {
		Log::quell();		// stops global logging until Log::restore()
	}

	Log::info  ("message 3\n");
	Log::debug ("message 4\n");
}

void function3 (void)
{
	Log::scope t ("message");
	LOG_SCOPE;
	LOG_SCOPE ("message");

	Log::error << "message";

	Log::error.format ("number %d", 42);
}

void function4 (void)
{
#define log_macro1(ARGS...)					\
	{							\
		Log::Function = __PRETTY_FUNCTION__;		\
		Log::File     = __FILE__;			\
		Log::Line     = __LINE__;			\
	}							\
	Log::Stream

	log_macro1 << "message";

#define log_macro1(ARGS...)					\

	log_macro2 ("%ld", number);
}

//int log_info (const char* format, ...) __attribute__ ((format (printf, 1, 2)));

inline log_method (void)
{
}

inline log_method (...)
{
}

void function5 (void)
{
#define log_macro1(ARGS...)					\
	{							\
		Log::Function = __PRETTY_FUNCTION__;		\
		Log::File     = __FILE__;			\
		Log::Line     = __LINE__;			\
	}							\
	Log::Stream

	log_macro1 << "message";

#define log_macro1(ARGS...)					\

	log_macro2 ("%ld", number);
}

