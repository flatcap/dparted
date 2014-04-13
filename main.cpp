/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <memory>
#include <ctime>

#ifdef DP_GUI
#include "gui_app.h"
#else
#include "text_app.h"
#endif
#include "log.h"
#include "log_trace.h"
#include "utils.h"

void
pink_log (Severity UNUSED(level), const char* UNUSED(function), const char* UNUSED(file), int UNUSED(line), const char* message)
{
	fprintf (stdout, "\033[38;5;200m%s\033[0m\n", message);
}

int
main (int argc, char *argv[])
{
	srandom (time (nullptr));

	//log_init (Severity::Hex,  log_stdout);
	log_init (Severity::AllDebug,  log_stdout);
	log_init (~Severity::AllDebug, log_stdout);
	//log_init (Severity::Ctor | Severity::Dtor, pink_log);

	int status = 0;

#ifdef DP_GUI
	gui_app = std::make_shared<GuiApp>();
	main_app = gui_app;
	status = gui_app->run (argc, argv);
#else
	text_app = std::make_shared<TextApp>();
	main_app = text_app;
	status = text_app->run (argc, argv);
#endif
	main_app = nullptr;

	log_close();
	return status;
}

