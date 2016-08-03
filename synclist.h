#ifndef SYNCLIST_H
#define SYNCLIST_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct synclist
{
  int max_size;
  int max_string_size;
  char **element;
  pthread_mutex_t mutex;
  pthread_cond_t read_done,write_done;
  int size;
};

void synclist_init(struct synclist *o,
           int max_size,int max_string_size)
{
  int i;
  pthread_mutex_init(&o->mutex,NULL);
  pthread_cond_init(&o->read_done,NULL);
  pthread_cond_init(&o->write_done,NULL);
  o->max_size = max_size;
  o->max_string_size = max_string_size;
  o->element = (char **)malloc(o->max_size*sizeof(char *));
  for(i=0;i<o->max_size;i++) {
    o->element[i] = (char *)malloc(o->max_string_size*sizeof(char));
  }
  o->size = 0;
}

void synclist_destroy(struct synclist *o)
{
  int i;
  pthread_mutex_lock(&o->mutex);
  pthread_mutex_destroy(&o->mutex);
  pthread_cond_destroy(&o->read_done);
  pthread_cond_destroy(&o->write_done);
  for(i=0;i<o->max_size;i++) {
    free(o->element[i]);
  }
  free(o->element);
}

void synclist_push(struct synclist *o,char *element)
{
  pthread_mutex_lock(&o->mutex);
  while ( o->size >= (o->max_size) ) {
    /* List is full, waiting for read_done */
    pthread_cond_wait(&o->read_done,&o->mutex);
  }
  /* Write and unlock */
  strncpy(o->element[o->size],element,o->max_string_size);
  o->size++;
  pthread_cond_signal(&o->write_done);
  pthread_mutex_unlock(&o->mutex);
}

void synclist_pop(struct synclist *o,char *element)
{
  pthread_mutex_lock(&o->mutex);
  while ( o->size <= 0 ) {
    /* Empty list, waiting for write_done */
    pthread_cond_wait(&o->write_done,&o->mutex);
  }
  /* Read and unlock */
  strncpy(element,o->element[o->size-1],o->max_string_size);
  o->size--;
  pthread_cond_signal(&o->read_done);
  pthread_mutex_unlock(&o->mutex);
}

#endif
