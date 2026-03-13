#include "hashmap.h"

// like constructor
void setNode(node *node, char *key, char *value) {
  node->key = key;
  node->value = value;
  node->next = NULL;
  return;
};

// like constructor
void initializeHashMap(hashMap *mp) {
  // Default capacity in this case
  mp->capacity = 100;
  mp->numOfElements = 0;

  // array of size = 1
  mp->arr = (node **)malloc(sizeof(node *) * mp->capacity);
  return;
}

int hashFunction(hashMap *mp, char *key) {
  int bucketIndex;
  int sum = 0, factor = 31;
  for (int i = 0; i < strlen(key); i++) {

    // sum = sum + (ascii value of
    // char * (primeNumber ^ x))...
    // where x = 1, 2, 3....n
    sum = ((sum % mp->capacity) + (((int)key[i]) * factor) % mp->capacity) %
          mp->capacity;

    // factor = factor * prime
    // number....(prime
    // number) ^ x
    factor = ((factor % __INT16_MAX__) * (31 % __INT16_MAX__)) % __INT16_MAX__;
  }

  bucketIndex = sum;
  return bucketIndex;
}

void insert(hashMap *mp, char *key, char *value) {
  // Getting bucket index for the given
  // key - value pair
  int bucketIndex = hashFunction(mp, key);
  node *newNode = (node *)malloc(

      // Creating a new node
      sizeof(node));

  // Setting value of node
  setNode(newNode, key, value);

  // Bucket index is empty....no collision
  if (mp->arr[bucketIndex] == NULL) {
    mp->arr[bucketIndex] = newNode;
  }

  // Collision
  else {

    // Adding newNode at the head of
    // linked list which is present
    // at bucket index....insertion at
    // head in linked list
    newNode->next = mp->arr[bucketIndex];
    mp->arr[bucketIndex] = newNode;
  }
  return;
}

void delete(hashMap *mp, char *key) {
  // Getting bucket index for the
  // given key
  int bucketIndex = hashFunction(mp, key);

  node *prevNode = NULL;

  // Points to the head of
  // linked list present at
  // bucket index
  node *currNode = mp->arr[bucketIndex];

  while (currNode != NULL) {

    // Key is matched at delete this
    // node from linked list
    if (strcmp(key, currNode->key) == 0) {

      // Head node
      // deletion
      if (currNode == mp->arr[bucketIndex]) {
        mp->arr[bucketIndex] = currNode->next;
      }

      // Last node or middle node
      else {
        prevNode->next = currNode->next;
      }
      free(currNode);
      break;
    }
    prevNode = currNode;
    currNode = currNode->next;
  }
  return;
}

char *search(hashMap *mp, char *key) {
  // Getting the bucket index
  // for the given key
  int bucketIndex = hashFunction(mp, key);

  // Head of the linked list
  // present at bucket index
  node *bucketHead = mp->arr[bucketIndex];
  while (bucketHead != NULL) {

    // Key is found in the hashMap
    if (!strcmp(bucketHead->key, key)) {
      return bucketHead->value;
    }
    bucketHead = bucketHead->next;
  }

  // If no key found in the hashMap
  // equal to the given key
  char *errorMssg = (char *)malloc(sizeof(char) * 25);
  errorMssg = "Oops! No data found.\n";
  return errorMssg;
}
