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
	srandom (time (nullptr));

	LogHandlerPtr log_out = LogHandler::create (stdout);
	log_out->start (
		Severity::Warning		|
		Severity::Verbose		|
		Severity::User			|
		Severity::Progress		|
		Severity::Quiet			|
		// Severity::Command		|
		// Severity::Debug			|
		// Severity::CommandIn		|
		// Severity::CommandOut		|
		// Severity::IoIn			|
		// Severity::IoOut			|
		// Severity::Dot			|
		// Severity::Hex			|
		// Severity::ConfigRead		|
		// Severity::ConfigWrite		|
		// Severity::Enter			|
		// Severity::Leave			|
		// Severity::File			|
		// Severity::Ctor			|
		// Severity::Dtor			|
		// Severity::Utils			|
		Severity::SystemEmergency
	);

	LogHandlerPtr log_red = LogHandler::create (stdout);
	// log_red->foreground = 209;
	log_red->start (Severity::Code);

#if 0
	LogHandlerPtr log_green = LogHandler::create (stdout);
	// log_green->foreground = 118;
	log_green->start (Severity::Trace);
#endif

#if 0
	LogHandlerPtr log_yellow = LogHandler::create (stdout);
	log_yellow->foreground = 226;
	log_yellow->start (Severity::Listener);
#endif

	LogHandlerPtr log_cyan = LogHandler::create (stdout);
	// log_cyan->foreground = 45;
	log_cyan->start (Severity::Info);

	LogHandlerPtr log_crit = LogHandler::create (stdout);
	log_crit->background = 196;
	log_crit->foreground = 15;
	log_crit->start (
		Severity::SystemAlert		|
		Severity::Critical		|
		Severity::Error			|
		Severity::Perror
	);

#if 0
	LogHandlerPtr log_thread = LogHandler::create (stdout);
	log_thread->foreground = 141;
	log_thread->start (
		Severity::ThreadStart	|
		Severity::ThreadEnd
	);
#endif

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

	// log_thread->stop();
	log_crit->stop();
	log_cyan->stop();
	// log_yellow->stop();
	// log_green->stop();
	log_red->stop();
	log_out->stop();

	return status;
}

