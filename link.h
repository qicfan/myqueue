#ifndef QUEUELINK
#define QUEUELINK
// queue data struct
typedef struct queue_link {
  char * value;
  int size;
  struct queue_link * next;
} queue;
// queue entry
typedef struct queue_entry {
  struct queue_link * head;
  struct queue_link * foot;
  int size;
} queue_entry;
// insert into queue
int queue_set(queue_entry * qe, const char * string);
// get data from queue
char * queue_get(queue_entry * qe);
#endif
