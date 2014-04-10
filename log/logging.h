log_handler log_handler_syslog  __attribute__((format(printf, 6, 0)));
log_handler log_handler_fprintf __attribute__((format(printf, 6, 0)));
log_handler log_handler_null    __attribute__((format(printf, 6, 0)));
log_handler log_handler_stdout  __attribute__((format(printf, 6, 0)));
log_handler log_handler_outerr  __attribute__((format(printf, 6, 0)));
log_handler log_handler_stderr  __attribute__((format(printf, 6, 0)));

int log_redirect(const char *function, const char *file, int line, u32 level, const char *format, ...) __attribute__((format(printf, 6, 7)));

#define LOG_LEVEL_DEBUG		(1 <<  0) /* x = 42 */
#define LOG_LEVEL_TRACE		(1 <<  1) /* Entering function x() */
#define LOG_LEVEL_QUIET		(1 <<  2) /* Quietable output */
#define LOG_LEVEL_INFO		(1 <<  3) /* Volume needs defragmenting */
#define LOG_LEVEL_VERBOSE	(1 <<  4) /* Forced to continue */
#define LOG_LEVEL_PROGRESS	(1 <<  5) /* 54% complete */
#define LOG_LEVEL_WARNING	(1 <<  6) /* You should backup before starting */
#define LOG_LEVEL_ERROR		(1 <<  7) /* Operation failed, no damage done */
#define LOG_LEVEL_PERROR	(1 <<  8) /* Message : standard error description */
#define LOG_LEVEL_CRITICAL	(1 <<  9) /* Operation failed,damage may have occurred */
#define LOG_LEVEL_ENTER		(1 << 10) /* Enter a function */
#define LOG_LEVEL_LEAVE		(1 << 11) /* Leave a function  */

#define LOG_FLAG_PREFIX		(1 << 0) /* Prefix messages with "ERROR: ", etc */
#define LOG_FLAG_FILENAME	(1 << 1) /* Show the file origin of the message */
#define LOG_FLAG_LINE		(1 << 2) /* Show the line number of the message */
#define LOG_FLAG_FUNCTION	(1 << 3) /* Show the function name containing the message */
#define LOG_FLAG_ONLYNAME	(1 << 4) /* Only display the filename, not the pathname */

#define log_critical(FORMAT, ARGS...) log_redirect(__FUNCTION__,__FILE__,__LINE__,  LOG_LEVEL_CRITICAL,FORMAT,##ARGS)
#define log_error(FORMAT, ARGS...)    log_redirect(__FUNCTION__,__FILE__,__LINE__,  LOG_LEVEL_ERROR,FORMAT,##ARGS)
#define log_info(FORMAT, ARGS...)     log_redirect(__FUNCTION__,__FILE__,__LINE__,  LOG_LEVEL_INFO,FORMAT,##ARGS)
#define log_perror(FORMAT, ARGS...)   log_redirect(__FUNCTION__,__FILE__,__LINE__,  LOG_LEVEL_PERROR,FORMAT,##ARGS)
#define log_progress(FORMAT, ARGS...) log_redirect(__FUNCTION__,__FILE__,__LINE__,  LOG_LEVEL_PROGRESS,FORMAT,##ARGS)
#define log_quiet(FORMAT, ARGS...)    log_redirect(__FUNCTION__,__FILE__,__LINE__,  LOG_LEVEL_QUIET,FORMAT,##ARGS)
#define log_verbose(FORMAT, ARGS...)  log_redirect(__FUNCTION__,__FILE__,__LINE__,  LOG_LEVEL_VERBOSE,FORMAT,##ARGS)
#define log_warning(FORMAT, ARGS...)  log_redirect(__FUNCTION__,__FILE__,__LINE__,  LOG_LEVEL_WARNING,FORMAT,##ARGS)

#ifdef DEBUG
#define log_debug(FORMAT, ARGS...) log_redirect(__FUNCTION__,__FILE__,__LINE__,LOG_LEVEL_DEBUG,FORMAT,##ARGS)
#define log_trace(FORMAT, ARGS...) log_redirect(__FUNCTION__,__FILE__,__LINE__,LOG_LEVEL_TRACE,FORMAT,##ARGS)
#define log_enter(FORMAT, ARGS...) log_redirect(__FUNCTION__,__FILE__,__LINE__,LOG_LEVEL_ENTER,FORMAT,##ARGS)
#define log_leave(FORMAT, ARGS...) log_redirect(__FUNCTION__,__FILE__,__LINE__,LOG_LEVEL_LEAVE,FORMAT,##ARGS)
#else
#define log_debug(FORMAT, ARGS...)do {} while (0)
#define log_trace(FORMAT, ARGS...)do {} while (0)
#define log_enter(FORMAT, ARGS...)do {} while (0)
#define log_leave(FORMAT, ARGS...)do {} while (0)
#endif /* DEBUG */
