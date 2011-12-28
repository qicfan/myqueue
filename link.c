#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "link.h"

int queue_set(struct queue_entry * qe, const char * string) {
	queue * q = (queue *)malloc(sizeof(queue));
	int size = strlen(string);
	q->value = (char *)malloc(size + 1);
	strcpy(q->value, string);
	q->value[size] = '\0';
	q->size = size;
	q->next = NULL;
	if (qe->head == NULL) {
		qe->head = q;
	}
    if (qe->foot != NULL) {
      qe->foot->next = q;
    }
	qe->foot = q;
	qe->size ++;
	return 0;
}

char * queue_get(struct queue_entry * qe) {
	if (qe->head == NULL) {
      return NULL;
    }
	queue * q = qe->head;
    char * return_value = q->value;
    qe->head = q->next;
    if (NULL == qe->head && qe->head == qe->foot) {
      qe->foot = NULL;
    }
    qe->size --;
    free(q);
    printf("free(q) %s\n", q);
    q = NULL;
    return return_value;
}
