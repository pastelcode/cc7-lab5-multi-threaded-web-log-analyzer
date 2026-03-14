#include "hashmap.h"
#include "log_processor.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096
#define THREADS_COUNT 4

typedef struct {
  long start_byte;
  long end_byte;
  const char *filename;
} ThreadArgs;

pthread_t thread_ids[THREADS_COUNT];
pthread_mutex_t lock;

hashMap *ips_map;
hashMap *urls_map;

int total_unique_ips_count;
char *most_visited_url;
int http_errors_count;

void record_ip(char *ip) {
  char new_value[16];

  pthread_mutex_lock(&lock);

  char *current_value_string = search(ips_map, ip);
  if (strcmp(current_value_string, "Oops! No data found.\n") == 0) {
    insert(ips_map, strdup(ip), strdup("1"));
    total_unique_ips_count++;
  } else {
    int current_value = atoi(current_value_string);
    sprintf(new_value, "%d", current_value + 1);
    insert(ips_map, strdup(ip), strdup(new_value));
  }

  pthread_mutex_unlock(&lock);
}

void record_url(char *url) {
  char new_value[16];

  pthread_mutex_lock(&lock);

  char *current_value_string = search(urls_map, url);
  char *url_copy = strdup(url);

  if (strcmp(current_value_string, "Oops! No data found.\n") == 0) {
    insert(urls_map, url_copy, strdup("1"));
    if (most_visited_url == NULL) {
      most_visited_url = url_copy;
    }
  } else {
    int current_value = atoi(current_value_string);
    sprintf(new_value, "%d", current_value + 1);
    insert(urls_map, url_copy, strdup(new_value));

    int occurrences = current_value + 1;
    int most_visited_occurrences = atoi(search(urls_map, most_visited_url));
    if (occurrences > most_visited_occurrences) {
      most_visited_url = url_copy;
    }
  }

  pthread_mutex_unlock(&lock);
}

void record_error(void) {
  pthread_mutex_lock(&lock);
  http_errors_count++;
  pthread_mutex_unlock(&lock);
}

void *process_log_chunk(void *arg) {
  ThreadArgs *args = (ThreadArgs *)arg;
  char buffer[BUFFER_SIZE];

  FILE *fp = fopen(args->filename, "r");
  if (fp == NULL) {
    perror("Error opening file");
    return NULL;
  }

  fseek(fp, args->start_byte, SEEK_SET);

  // If not starting at the beginning of the file, the seek position may land
  // in the middle of a line. Check the preceding byte: if it is not a newline,
  // we are mid-line and must discard the rest of that line (already handled by
  // the previous thread). If it IS a newline, we are already at a line start.
  if (args->start_byte > 0) {
    fseek(fp, args->start_byte - 1, SEEK_SET);
    char prev = (char)fgetc(fp); // advances fp back to start_byte
    if (prev != '\n') {
      fgets(buffer, BUFFER_SIZE, fp); // discard partial line
    }
  }

  while (ftell(fp) <= args->end_byte) {
    if (fgets(buffer, BUFFER_SIZE, fp) == NULL) {
      break;
    }

    LogEntry *log_entry = process_log_entry(buffer);
    if (log_entry == NULL) {
      continue;
    }

    record_ip(log_entry->ip_address);
    record_url(log_entry->request_url);

    if (log_entry->response_code >= 400 && log_entry->response_code <= 599) {
      record_error();
    }

    free(log_entry);
  }

  fclose(fp);
  return NULL;
}

int main(int argc, char *argv[]) {
  const char *filename = (argc > 1) ? argv[1] : "access.log";

  ips_map = (hashMap *)malloc(sizeof(hashMap));
  urls_map = (hashMap *)malloc(sizeof(hashMap));
  initializeHashMap(ips_map);
  initializeHashMap(urls_map);

  pthread_mutex_init(&lock, NULL);

  // Determine file size so each thread can be assigned a byte-range chunk
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("Error opening file");
    return 1;
  }
  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  fclose(fp);

  long chunk_size = file_size / THREADS_COUNT;

  ThreadArgs thread_args[THREADS_COUNT];
  for (int i = 0; i < THREADS_COUNT; i++) {
    thread_args[i].filename = filename;
    thread_args[i].start_byte = i * chunk_size;
    thread_args[i].end_byte =
        (i == THREADS_COUNT - 1) ? file_size - 1 : (i + 1) * chunk_size - 1;
  }

  // Create all threads first so they run in parallel
  for (int i = 0; i < THREADS_COUNT; i++) {
    pthread_create(&thread_ids[i], NULL, process_log_chunk, &thread_args[i]);
  }

  // Then wait for all threads to finish
  for (int i = 0; i < THREADS_COUNT; i++) {
    pthread_join(thread_ids[i], NULL);
  }

  printf("Total Unique IPs: %d\n", total_unique_ips_count);

  if (most_visited_url != NULL) {
    int most_visited_occurrences = atoi(search(urls_map, most_visited_url));
    printf("Most Visited URL: %s (%d times)\n", most_visited_url,
           most_visited_occurrences);
  }

  printf("HTTP Errors: %d\n", http_errors_count);

  pthread_mutex_destroy(&lock);
  return 0;
}
