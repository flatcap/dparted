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

int
main (int argc, char *argv[])
{
	LogHandlerPtr log_out  = LogHandler::create (stdout);
	LogHandlerPtr log_file = LogHandler::create ("logfile.txt", false);
	LogHandlerPtr log_tty  = LogHandler::create ("/dev/pts/0",  true);

	log_out->foreground = 226;
	log_out->timestamp  = true;
	log_tty->background = 208;

	log_add_handler (Severity::AllMessages, log_out);
	log_add_handler (Severity::AllDebug,    log_file);
	log_add_handler (Severity::AllDebug,    log_tty);

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

	return status;
}

