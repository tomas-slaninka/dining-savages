#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <unistd.h>

typedef struct {
	int servings;
	sem_t * dataCopied;
	sem_t * emptyPot;
	sem_t * fullPot;
	sem_t * mutex;
} Data;

int cook(void *inputData) {
	Data data = *((Data*)inputData);
	
	assert(sem_post(data.dataCopied) == 0);
	
	while(1) {
		assert(sem_wait(data.emptyPot) == 0);
		printf("Cook is adding 10 portions\n");
		data.servings = 10;
		assert(sem_post(data.fullPot) == 0);
		  
	}
}

int savage(void *inputData) {
	Data data = *((Data*)inputData);
  
	assert(sem_post(data.dataCopied) == 0);
	
	while(1) {
    usleep(rand() % 1000);
		assert(sem_wait(data.mutex) == 0);
		
		if (data.servings == 0){
			assert(sem_post(data.emptyPot) == 0);
      assert(sem_wait(data.fullPot) == 0);
		}
    printf("Savage is eating...\n");
    data.servings -= 1;
		
		assert(sem_post(data.mutex) == 0);
		  
	}  
}

int main() {
	int i = 0;
	pthread_t id[5];

	Data data;
	sem_t dataCopied, mutex, emptyPot, fullPot;
  
	data.servings = 0;

	assert(sem_init(&dataCopied, 0, 0) == 0);
	data.dataCopied = &dataCopied;
	assert(sem_init(&mutex, 0, 1) == 0);
	data.mutex = &mutex;
  
	assert(sem_init(&emptyPot, 0, 0) == 0);
	data.emptyPot = &emptyPot;
  
	assert(sem_init(&fullPot, 0, 0) == 0);
	data.fullPot = &fullPot;

	assert(pthread_create(&id[0], NULL, cook, (void*)&data) == 0);
	assert(sem_wait(&dataCopied) == 0);

  for (i = 1; i < 5; i++){
    assert(pthread_create(&id[1], NULL, savage, (void*)&data) == 0);
    assert(sem_wait(&dataCopied) == 0);
  
  }

	for (i = 0; i < 5; i++) {
		assert(pthread_join(id[i], NULL) == 0);
	}

	return 0;
}
