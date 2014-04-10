log_handler log_handler_syslog  __attribute__((format(printf, 6, 0)));
log_handler log_handler_fprintf __attribute__((format(printf, 6, 0)));
log_handler log_handler_null    __attribute__((format(printf, 6, 0)));
log_handler log_handler_stdout  __attribute__((format(printf, 6, 0)));
log_handler log_handler_outerr  __attribute__((format(printf, 6, 0)));
log_handler log_handler_stderr  __attribute__((format(printf, 6, 0)));
