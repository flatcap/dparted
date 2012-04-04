
#include "log.h"	// global log object

namespace Log
{
	log.debug (...);
	log.info (...);
}

void function1 (void)
{
	log (Log::log);		// copy global object

	log.debug ("message 1\n");
	log.info  ("message 2\n");

	if (condition) {
		log.quell();		// stops logging for rest of function1
	}

	log.info  ("message 3\n");
	log.debug ("message 4\n");
}

void function2 (void)
{
	log.debug ("message 1\n");
	log.info  ("message 2\n");

	if (condition) {
		log.quell();		// stops global logging until log.restore()
	}

	log.info  ("message 3\n");
	log.debug ("message 4\n");
}

