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

int
main (int argc, char *argv[])
{
	srandom (time (nullptr));

#if 0
	LogObject my_log;
	my_log.reset_tty = true;
	my_log.open_file ("/dev/pts/1");

	log_callback_t my_log_cb = (log_callback_t) std::bind(&LogObject::log_line, &my_log, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
#endif

	log_init (Severity::AllDebug,  log_stdout);
	log_init (~Severity::AllDebug, log_stdout);
	// log_init (Severity::Ctor | Severity::Dtor, my_log_cb);

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

	log_close();
	return status;
}

