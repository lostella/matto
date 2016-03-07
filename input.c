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

void * i_input_listener(void * param) {
	char * s;
	while (pthread_mutex_trylock(&shutdown) == EBUSY) {
		pthread_mutex_lock(&waiting);
		s = fgets(buffer, 256, stdin);		/* si mette in attesa di un input */
		pthread_mutex_unlock(&waiting);
		pthread_mutex_unlock(&full);	/* comunica buffer pieno */
		pthread_mutex_lock(&empty);		/* attende buffer vuoto */
	}
	pthread_mutex_unlock(&done);
	return NULL;
}

int i_is_waiting_input() {
	/* ritorna 1 se il thread è in attesa dell'input */
	int r = pthread_mutex_trylock(&waiting);
	if (r != EBUSY)
		pthread_mutex_unlock(&waiting);
	return r == EBUSY;
}

void i_start_input_thread() {
	/* inizializza i lock */
	pthread_mutex_init(&empty, 0);
	pthread_mutex_init(&full, 0);
	pthread_mutex_init(&done, 0);
	pthread_mutex_init(&shutdown, 0);
	pthread_mutex_init(&waiting, 0);
	/* inizializza i lock mutex */
	pthread_mutex_lock(&full);
	pthread_mutex_lock(&empty);
	/* il thread deve ancora partire (non si può dire che abbia finito!) */
	pthread_mutex_lock(&done);
	pthread_mutex_lock(&shutdown);
	/* il thread prima di partire NON è in attesa */
	pthread_mutex_unlock(&waiting);
	/* fa partire il thread */
	pthread_create(&tid, 0, i_input_listener, 0);
}

void i_stop_input_thread() {
	/* comunica al thread che deve terminare */
	pthread_mutex_unlock(&shutdown);
	/* sblocca eventuale lock sul buffer vuoto */
	pthread_mutex_unlock(&empty);
	/* attende la terminazione del thread */
	pthread_mutex_lock(&done);
	/* distrugge i mutex lock */
	pthread_mutex_destroy(&empty);
	pthread_mutex_destroy(&full);
	pthread_mutex_destroy(&done);
	pthread_mutex_destroy(&shutdown);
	pthread_mutex_destroy(&waiting);
}

int i_v_empty() {
	/* comunica buffer vuoto */
	if (i_is_waiting_input() == 0)
		return pthread_mutex_unlock(&empty);
	return 0;
}

int i_p_full() {
	/* si mette in attesa del buffer pieno */
	return pthread_mutex_lock(&full);
}

int i_tryp_full() {
	/* controlla se il buffer è pieno */
	return pthread_mutex_trylock(&full);
}

char * i_get_buffer() {
	return buffer;
}
