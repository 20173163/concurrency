#include <stdio.h>
#include <stdlib.h>
#define MAX 10

int loops;

int buffer[MAX];
int fill = 0;
int use = 0;
int count = 0;

pthread_cond_t empty, fill_c;
pthread_mutex_t mutex;

int psum = 0;
int csum = 0;

void put(int value);
int get();
void *producer(void *avg);
void *consumer(void *avg);

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stderr, "usage : bounded_buffer <value>\n");
		exit(1);
	}
	loops = atoi(argv[1]);
	pthread_t p, c;
        
	pthread_create(&p, NULL, producer, NULL);
	pthread_create(&c, NULL, consumer, NULL);
	pthread_join(p, NULL);
	pthread_join(c, NULL);

	if(psum == csum)
		printf("Test OK\n");
	else
		printf("Wrong\n");
        
	return 0;
}

void put(int value) {
	buffer[fill] = value;
	fill = (fill + 1) % MAX;
	count++;
	psum += (value - fill);
}

int get() {
	int tmp = buffer[use];
	use = (use + 1) % MAX;
	count--;
	csum += (tmp - use);
	return tmp;
}

void *producer(void *arg) {
	int i;
	for(i=0; i<loops; i++) {
		pthread_mutex_lock(&mutex);
		while(count == MAX)
			pthread_cond_wait(&empty, &mutex);
		put(i);
		pthread_cond_signal(&fill_c);
		pthread_mutex_unlock(&mutex);
	}
}

void *consumer(void *arg) {
	int i;
	for(i=0; i<loops; i++) {
		pthread_mutex_lock(&mutex);
		while(count == 0)
			pthread_cond_wait(&fill_c, &mutex);
		int tmp = get();
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mutex);
		printf("%d\n", tmp);
	}
}
