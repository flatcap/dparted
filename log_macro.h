#ifndef _LOG_MACRO_H_
#define _LOG_MACRO_H_

#include "log_severity.h"

#ifdef DP_LOG_CHECK
#define log_system_emergency log_redirect
#define log_system_alert     log_redirect
#define log_critical         log_redirect
#define log_error            log_redirect
#define log_perror           log_redirect
#define log_code             log_redirect
#define log_warning          log_redirect
#define log_verbose          log_redirect
#define log_user             log_redirect
#define log_info             log_redirect
#define log_progress         log_redirect
#define log_quiet            log_redirect
#define log_command          log_redirect
#define log_debug            log_redirect
#define log_trace            log_redirect
#define log_command_in       log_redirect
#define log_command_out      log_redirect
#define log_io_in            log_redirect
#define log_io_out           log_redirect
#define log_dot              log_redirect
#define log_hex              log_redirect
#define log_config_read      log_redirect
#define log_config_write     log_redirect
#define log_enter            log_redirect
#define log_leave            log_redirect
#define log_file             log_redirect
#define log_ctor             log_redirect
#define log_dtor             log_redirect
#define log_thread           log_redirect
#else
// Log level: EMERG
#define log_system_emergency(...) log_redirect(Severity::SystemEmergency,__PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: ALERT
#define log_system_alert(...)     log_redirect(Severity::SystemAlert,    __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: CRIT
#define log_critical(...)         log_redirect(Severity::Critical,       __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: ERR
#define log_error(...)            log_redirect(Severity::Error,          __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_perror(...)           log_redirect(Severity::Perror,         __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_code(...)             log_redirect(Severity::Code,           __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: WARNING
#define log_warning(...)          log_redirect(Severity::Warning,        __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: NOTICE
#define log_verbose(...)          log_redirect(Severity::Verbose,        __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_user(...)             log_redirect(Severity::User,           __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: INFO
#define log_info(...)             log_redirect(Severity::Info,           __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_progress(...)         log_redirect(Severity::Progress,       __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_quiet(...)            log_redirect(Severity::Quiet,          __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_command(...)          log_redirect(Severity::Command,        __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
// Log level: DEBUG
#define log_debug(...)            log_redirect(Severity::Debug,          __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_trace(...)            log_redirect(Severity::Trace,          __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_command_in(...)       log_redirect(Severity::CommandIn,      __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_command_out(...)      log_redirect(Severity::CommandOut,     __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_io_in(...)            log_redirect(Severity::IoIn,           __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_io_out(...)           log_redirect(Severity::IoOut,          __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_dot(...)              log_redirect(Severity::Dot,            __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_hex(...)              log_redirect(Severity::Hex,            __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_config_read(...)      log_redirect(Severity::ConfigRead,     __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_config_write(...)     log_redirect(Severity::ConfigWrite,    __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_enter(...)            log_redirect(Severity::Enter,          __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_leave(...)            log_redirect(Severity::Leave,          __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_file(...)             log_redirect(Severity::File,           __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_ctor(...)             log_redirect(Severity::Ctor,           __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_dtor(...)             log_redirect(Severity::Dtor,           __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#define log_thread(...)           log_redirect(Severity::Thread,         __PRETTY_FUNCTION__,__FILE__,__LINE__,__VA_ARGS__)
#endif

#endif // _LOG_MACRO_H_

