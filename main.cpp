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
#include <functional>

#ifdef DP_GUI
#include "gui_app.h"
#else
#include "text_app.h"
#endif
#include "log.h"
#include "utils.h"
#include "log_handler.h"

void
log_pink (Severity UNUSED(level), const char* UNUSED(function), const char* UNUSED(file), int UNUSED(line), const char* message)
{
	fprintf (stdout, "\033[38;5;207m%s\033[0m\n", message);
}

int
main (int argc, char *argv[])
{
	LogHandlerPtr log_out = LogHandler::create (stdout);
	if (log_out) {
		log_out->foreground = 226;
		log_out->timestamp  = true;
		log_out->show_level = true;
		log_out->start (Severity::AllMessages);
	}

	LogHandlerPtr log_file = LogHandler::create ("logfile.txt", false);
	if (log_file) {
		log_file->start (Severity::AllDebug);
	}

	LogHandlerPtr log_tty = LogHandler::create ("/dev/pts/0", true);
	if (log_tty) {
		log_tty->background = 208;
		log_tty->start (Severity::AllDebug);
	}

	int handle = log_add_handler (log_pink, Severity::Code);

	srandom (time (nullptr));

	int status = 0;

#ifdef DP_GUI
	gui_app = std::make_shared<GuiApp>();
	main_app = gui_app;
	status = gui_app->run (argc, argv);
	gui_app = nullptr;
#else
	text_app = std::make_shared<TextApp>();
	main_app = text_app;
	status = text_app->run (argc, argv);
	text_app = nullptr;
#endif
	main_app = nullptr;

	log_remove_handler (handle);
	if (log_out)  log_out->stop();
	if (log_file) log_file->stop();
	if (log_tty)  log_tty->stop();

	return status;
}

