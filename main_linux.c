#include "hashmap.h"
#include "log_processor.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define THREADS_COUNT 1

pthread_t thread_ids[THREADS_COUNT];
pthread_mutex_t lock;

hashMap *ips_map;
hashMap *urls_map;
hashMap *error_codes_map;
FILE *file_pointer;

int total_unique_ips_count;
char *most_visited_url;
int http_errors_count;

void record_ip(char *ip) {
  char new_value[16];

  pthread_mutex_lock(&lock);

  char *current_value_string = search(ips_map, ip);
  if (strcmp(current_value_string, "Oops! No data found.\n") == 0) {
    insert(ips_map, ip, "1");
    total_unique_ips_count++;
  } else {
    int current_value = atoi(current_value_string);
    sprintf(new_value, "%d", ++current_value);
    insert(ips_map, ip, new_value);
  }

  pthread_mutex_unlock(&lock);
}

void record_url(char *url) {
  char new_value[16];

  pthread_mutex_lock(&lock);

  char *current_value_string = search(urls_map, url);
  if (strcmp(current_value_string, "Oops! No data found.\n") == 0) {
    insert(urls_map, url, "1");
  } else {
    int current_value = atoi(current_value_string);
    sprintf(new_value, "%d", ++current_value);
    insert(urls_map, url, new_value);
  }

  if (most_visited_url == NULL) {
    most_visited_url = url;
  } else {
    int occurrences = atoi(search(urls_map, url));
    int most_visited_url_occurrences = atoi(search(urls_map, most_visited_url));
    if (occurrences > most_visited_url_occurrences) {
      most_visited_url = url;
    }
  }

  pthread_mutex_unlock(&lock);
}

void record_error_code(char *error_code) {

  pthread_mutex_lock(&lock);

  http_errors_count++;

  pthread_mutex_unlock(&lock);
}

void *process_log_entry_routine() {
  char buffer[BUFFER_SIZE];
  while (fgets(buffer, BUFFER_SIZE, file_pointer)) {
    printf("%s\n", buffer);
    LogEntry *log_entry = process_log_entry(buffer);

    record_ip(log_entry->ip_address);
    printf("here\n");
    record_url(log_entry->request_url);

    printf("here\n");

    if (log_entry->response_code >= 400 && log_entry->response_code <= 599) {
      char response_code_string[24];
      sprintf(response_code_string, "%d", log_entry->response_code);
      record_error_code(response_code_string);
    }
  }
  return NULL;
}

int main() {
  ips_map = (hashMap *)malloc(sizeof(hashMap));
  urls_map = (hashMap *)malloc(sizeof(hashMap));
  error_codes_map = (hashMap *)malloc(sizeof(hashMap));
  initializeHashMap(ips_map);
  initializeHashMap(urls_map);
  initializeHashMap(error_codes_map);

  file_pointer = fopen("access_short.log", "r");

  for (int index = 0; index < THREADS_COUNT; index++) {
    pthread_create(&thread_ids[index], NULL, process_log_entry_routine, NULL);
    pthread_join(thread_ids[index], NULL);
  }

  printf("Total Unique IPs: %d\n", total_unique_ips_count);

  int most_visited_url_occurrences = atoi(search(urls_map, most_visited_url));
  printf("Most Visited URL: %s (%d times)\n", most_visited_url,
         most_visited_url_occurrences);

  printf("HTTP Errors: %d\n", http_errors_count);

  fclose(file_pointer);
  return 0;
}

// int main() {
//   // LogEntry *log_entry = process_log_entry(
//   //     "192.168.1.3 - - [10/Feb/2024:10:20:43] \"DELETE /api/data\" 200");
//   //

//   FILE *file;
//   char buffer[BUFFER_SIZE];
//   size_t bytesRead;

//   // Open the file for reading
//   file = fopen("access_short.log", "rb");
//   if (file == NULL) {
//     perror("Error opening file");
//     return 1;
//   }

//   // Read and process the file in chunks
//   while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
//     // Process the buffer (you can replace or extend this part)
//     // For example, print the content of the buffer:
//     fwrite(buffer, 1, bytesRead, stdout);
//     printf("\n");
//   }

//   // Close the file
//   fclose(file);

//   return 0;
// }
