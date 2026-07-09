#include "utils.h"

void logging(LogLevel level, const char *msg, ...) {
  if (level < DEFAULT_LOG_LEVEL)
    return;
  switch (level) {
  case LOG_LEVEL_WARN:
    printf("%s[%s]%s - ", ANSI_COLOR_RED, "WARN", ANSI_COLOR_RESET);
    break;
  case LOG_LEVEL_INFO:
    printf("%s[%s]%s - ", ANSI_COLOR_GREEN, "INFO", ANSI_COLOR_RESET);
    break;
  case LOG_LEVEL_DEBUG:
    printf("%s[%s]%s - ", ANSI_COLOR_BLUE, "DEBUG", ANSI_COLOR_RESET);
    break;
  case LOG_LEVEL_TRACE:
    printf("%s[%s]%s - ", ANSI_COLOR_CYAN, "TRACE", ANSI_COLOR_RESET);
  default:
    break;
  }

  va_list args;
  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);
}
