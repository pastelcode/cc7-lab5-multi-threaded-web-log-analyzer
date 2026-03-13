#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linked List node
typedef struct node {
  // key is string
  char *key;

  // value is also string
  char *value;
  struct node *next;
} node;

// like constructor
void setNode(struct node *node, char *key, char *value);

typedef struct hashMap {
  // Current number of elements in hashMap
  // and capacity of hashMap
  int numOfElements, capacity;

  // hold base address array of linked list
  struct node **arr;
} hashMap;

// like constructor
void initializeHashMap(hashMap *mp);

int hashFunction(hashMap *mp, char *key);

void insert(hashMap *mp, char *key, char *value);

void delete(hashMap *mp, char *key);

char *search(hashMap *mp, char *key);

#endif /* HASHMAP_H */
