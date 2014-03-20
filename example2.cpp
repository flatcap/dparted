
#include "log.h"	// global log object

namespace Log
{
	debug (...);
	info (...);

	Object log;		// main log
}

void function1 (void)
{
	log (Log::log);		// copy global object

	Log::debug ("message 1\n");
	Log::info  ("message 2\n");

	if (condition) {
		Log::quell();		// stops logging for rest of function1
	}

	Log::info  ("message 3\n");
	Log::debug ("message 4\n");
}

void function2 (void)
{
	Log::debug ("message 1\n");
	Log::info  ("message 2\n");

	if (condition) {
		Log::quell();		// stops global logging until Log::restore()
	}

	Log::info  ("message 3\n");
	Log::debug ("message 4\n");
}

