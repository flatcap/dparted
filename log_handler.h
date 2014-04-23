#ifndef _LOG_HANDLER_H_
#define _LOG_HANDLER_H_

typedef std::function<void(Severity level, const char* function, const char* file, int line, const char* message)> log_handler_t;

class LogHandler
{
public:
	LogHandler (const char* filename);
	LogHandler (const std::string& filename);
	LogHandler (FILE* handle);
	LogHandler (std::ostream& output);
	LogHandler (log_handler_t callback);

	virtual ~LogHandler();

	void log_line (Severity level, const char* function, const char* file, int line, const char* message);

	void flush (void);
	void reset (void);

	bool timestamp      = false;
	bool show_level     = false;
	bool show_file_line = false;
	bool show_function  = false;
	bool reset_tty      = false;
	bool flush_line     = false;

	int  foreground = -1;
	int  background = -1;
	bool bold       = false;
	bool underline  = false;

protected:

};

#endif // _LOG_HANDLER_H_

