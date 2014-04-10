#ifndef _LOG_MACRO_H_
#define _LOG_MACRO_H_

#include "severity.h"

// Log level: EMERG
#define log_system_emergency(...) log_redirect(Severity::SystemEmergency,__FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: ALERT
#define log_system_alert(...)     log_redirect(Severity::SystemAlert,    __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: CRIT
#define log_critical(...)         log_redirect(Severity::Critical,       __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: ERR
#define log_error(...)            log_redirect(Severity::Error,          __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_perror(...)           log_redirect(Severity::Perror,         __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_code(...)             log_redirect(Severity::Code,           __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: WARNING
#define log_warning(...)          log_redirect(Severity::Warning,        __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: NOTICE
#define log_verbose(...)          log_redirect(Severity::Verbose,        __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_user(...)             log_redirect(Severity::User,           __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: INFO
#define log_info(...)             log_redirect(Severity::Info,           __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_progress(...)         log_redirect(Severity::Progress,       __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_quiet(...)            log_redirect(Severity::Quiet,          __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: DEBUG
#define log_debug(...)            log_redirect(Severity::Debug,          __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_trace(...)            log_redirect(Severity::Trace,          __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_command_in(...)       log_redirect(Severity::CommandIn,      __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_command_out(...)      log_redirect(Severity::CommandOut,     __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_io_in(...)            log_redirect(Severity::IoIn,           __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_io_out(...)           log_redirect(Severity::IoOut,          __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_dot(...)              log_redirect(Severity::Dot,            __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_hex(...)              log_redirect(Severity::Hex,            __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_config_read(...)      log_redirect(Severity::ConfigRead,     __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_config_write(...)     log_redirect(Severity::ConfigWrite,    __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_enter(...)            log_redirect(Severity::Enter,          __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_leave(...)            log_redirect(Severity::Leave,          __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_file(...)             log_redirect(Severity::File,           __FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)

#endif // _LOG_MACRO_H_

