#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define handler_error(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)

struct thread_attr {
	pthread_mutex_t mut;
	pthread_spinlock_t sp;
	pthread_rwlock_t rwlock;
	
	pthread_t tid;
	void* result;
};

static void* thread_start(void* arg){
	struct thread_attr *that = (struct thread_attr*)arg;
	int tid = that->tid;
	switch(tid){
	case 1: pthread_mutex_lock(&that->mut); sleep(10); pthread_mutex_unlock(&that->mut); break;
	case 2: pthread_spin_lock(&that->sp); sleep(10); pthread_spin_unlock(&that->sp); break;
	case 3: pthread_rwlock_rdlock(&that->rwlock); sleep(10); pthread_rwlock_unlock(&that->rwlock); pthread_rwlock_wrlock(&that->rwlock); sleep(5); pthread_rwlock_unlock(&that->rwlock); break;
	case 4: pthread_rwlock_wrlock(&that->rwlock); sleep(15); pthread_rwlock_unlock(&that->rwlock); pthread_rwlock_rdlock(&that->rwlock); pthread_rwlock_unlock(&that->rwlock); break;
	default: break;}
}

int main(int argc, char** argv) {
	FILE* fp;
	fp = fopen("/home/rostislav/main.pid", "w+");
	fprintf(fp, "%d", getpid());
	fclose(fp);

	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_spinlock_t sp;
	pthread_spin_init(&sp, PTHREAD_PROCESS_SHARED);
	pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

	pthread_attr_t attr;
	int s;
	s = pthread_attr_init(&attr);
	if(s!=0) handler_error(s, "attr_init");
	
	struct thread_attr that;

	that.tid = 1;
	that.mut = mut;
	s = pthread_create(&that.tid, &attr, &thread_start, &that.result);
	if(s!=0) handler_error(s, "create");
	pthread_mutex_lock(&mut);
	sleep(10);	
	pthread_mutex_unlock(&mut);

	that.tid = 2;
	that.sp = sp;
	s = pthread_create(&that.tid, &attr, &thread_start, &that.result);
	if(s!=0) handler_error(s, "create");
	pthread_spin_lock(&sp);
	sleep(10);
	pthread_spin_unlock(&sp);

	that.tid = 3;
	that.rwlock = rwlock;
	s = pthread_create(&that.tid, &attr, &thread_start, &that.result);
	if(s!=0) handler_error(s, "create");
	pthread_rwlock_rdlock(&rwlock);
	sleep(10);
	pthread_rwlock_unlock(&rwlock);

	that.tid = 4;
	s = pthread_create(&that.tid, &attr, &thread_start, &that.result);
	if(s!=0) handler_error(s, "create");
	pthread_rwlock_wrlock(&rwlock);
	sleep(10);
	pthread_rwlock_unlock(&rwlock);

	for(int i=1; i<5; i++){
		that.tid = i;
		s = pthread_join(that.tid, &that.result);
		if(s!=0) handler_error(s, "join");
	}

	pthread_mutex_destroy(&mut);
	pthread_spin_destroy(&sp);
	pthread_rwlock_destroy(&rwlock);
	
	exit(EXIT_SUCCESS);
}
