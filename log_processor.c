#include "log_processor.h"

LogEntry *process_log_entry(const char *log_line) {
  LogEntry *entry = (LogEntry *)malloc(sizeof(LogEntry));
  if (entry == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  sscanf(log_line, "%s - - [%[^]]] \"%s %[^\"]\" %i", entry->ip_address,
         entry->timestamp, entry->request_method, entry->request_url,
         &entry->response_code);

  return entry;
}
