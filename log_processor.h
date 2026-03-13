#ifndef LOG_PROCESSOR_H_
#define LOG_PROCESSOR_H_

#include <stdio.h>
#include <stdlib.h>

struct {
  char ip_address[16];
  char timestamp[21];
  char request_method[8];
  char request_url[256];
  int response_code;
} typedef LogEntry;

LogEntry *process_log_entry(const char *log_line);

#endif /* LOG_PROCESSOR_H_ */
