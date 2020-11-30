#include <errno.h>
#include <pthread.h>
#include <stdio.h>

char buffer[256] = "";

pthread_t tid;

pthread_mutex_t empty;
pthread_mutex_t full;
pthread_mutex_t done;
pthread_mutex_t shutdown;
pthread_mutex_t waiting;

void *i_input_listener(void *param) {
  char *s;
  while (pthread_mutex_trylock(&shutdown) == EBUSY) {
    pthread_mutex_lock(&waiting);
    s = fgets(buffer, 256, stdin); // wait for input
    pthread_mutex_unlock(&waiting);
    pthread_mutex_unlock(&full); // report full buffer
    pthread_mutex_lock(&empty);  // wait for empty buffer
  }
  pthread_mutex_unlock(&done);
  return NULL;
}

int i_is_waiting_input() {
  /* return 1 if waiting for input */
  int r = pthread_mutex_trylock(&waiting);
  if (r != EBUSY)
    pthread_mutex_unlock(&waiting);
  return r == EBUSY;
}

void i_start_input_thread() {
  /* initialize locks */
  pthread_mutex_init(&empty, 0);
  pthread_mutex_init(&full, 0);
  pthread_mutex_init(&done, 0);
  pthread_mutex_init(&shutdown, 0);
  pthread_mutex_init(&waiting, 0);
  pthread_mutex_lock(&full);
  pthread_mutex_lock(&empty);
  /* the thread has yet to start */
  pthread_mutex_lock(&done);
  pthread_mutex_lock(&shutdown);
  pthread_mutex_unlock(&waiting);
  /* start thread */
  pthread_create(&tid, 0, i_input_listener, 0);
}

void i_stop_input_thread() {
  /* tell the thread to shut down */
  pthread_mutex_unlock(&shutdown);
  /* unlock lock on buffer */
  pthread_mutex_unlock(&empty);
  /* wait for thread to finish */
  pthread_mutex_lock(&done);
  /* destroy mutex locks */
  pthread_mutex_destroy(&empty);
  pthread_mutex_destroy(&full);
  pthread_mutex_destroy(&done);
  pthread_mutex_destroy(&shutdown);
  pthread_mutex_destroy(&waiting);
}

int i_v_empty() {
  /* report empty buffer */
  if (i_is_waiting_input() == 0)
    return pthread_mutex_unlock(&empty);
  return 0;
}

int i_p_full() {
  /* wait for full buffer */
  return pthread_mutex_lock(&full);
}

int i_tryp_full() {
  /* check whether the buffer is full */
  return pthread_mutex_trylock(&full);
}

char *i_get_buffer() { return buffer; }
