#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#define handler_error(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mt = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t bt;

void* thread_start(void* arg){
	pthread_t* tid = (pthread_t*)arg;
	switch(*tid){
	case 1: pthread_cond_wait(&cond, &mt); break;
	case 2: pthread_barrier_wait(&bt); break;
	default: break;
	}
}

int main(int argc, char** argv) {
        FILE* fp;
        fp = fopen("/home/rostislav/main.pid", "w+");
        fprintf(fp, "%d", getpid());
        fclose(fp);

	pthread_attr_t attr;
        int s;
        s = pthread_attr_init(&attr);
        if(s!=0) handler_error(s, "attr_init");
	
	void* result;

	pthread_t tid = 1;
	s = pthread_create(&tid, &attr, &thread_start, &result);
	if(s!=0) handler_error(s, "create");
	tid = 2;
	s = pthread_create(&tid, &attr, &thread_start, &result);
	if(s!=0) handler_error(s, "create");

	sleep(10);
	s = pthread_cond_signal(&cond);	
	if(s!=0) handler_error(s, "condition signal");
	s = pthread_barrier_init(&bt, NULL, 1);
	if(s!=0) handler_error(s, "barrier sync");

	pthread_cond_destroy(&cond);
	pthread_barrier_destroy(&bt);
	pthread_mutex_destroy(&mt);
	
	tid = 1;
	pthread_join(tid, NULL);
	tid = 2;
	pthread_join(tid, NULL);
	
	exit(EXIT_SUCCESS);
}
