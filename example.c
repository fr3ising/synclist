#include "synclist.h"
#include <stdio.h>

#define LIST_SIZE 10
#define BUFFER_SIZE 1024
#define READERS 100

void *writer(void *mysl)
{
  int counter = 0;
  char msg[BUFFER_SIZE];
  struct synclist *sl;
  sl = (struct synclist *)mysl;
  while ( 1 ) {
    sprintf(msg,"message #%d",counter);
    counter++;
    synclist_push(sl,msg);
    printf("Writing msg %s (list size %d) (thread id %d)\n",
            msg,sl->size,abs((unsigned int)pthread_self()));
    fflush(stdout);
  }
}

void *reader(void *mysl)
{
  char msg[BUFFER_SIZE];
  struct synclist *sl;
  sl = (struct synclist *)mysl;
  while ( 1 ) {
    synclist_pop(sl,msg);
    printf("Reading msg %s (list size %d) (thread id %d)\n",
       msg,sl->size,abs((unsigned int)pthread_self()));
    fflush(stdout);
  }
}

int main(int argc,char *argv[])
{
  struct synclist sl;
  char buffer[BUFFER_SIZE];
  pthread_t t[READERS+1];
  int i;
  synclist_init(&sl,LIST_SIZE,BUFFER_SIZE);
  pthread_create(&t[0],NULL,writer,(void *)&sl);
  for(i=0;i<READERS;i++) {
    pthread_create(&t[i+1],NULL,reader,(void *)&sl);
  }
  while ( 1 ) {
    continue;
  }
  synclist_destroy(&sl);
  return 0;
}
