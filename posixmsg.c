#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>

int main(int argc, char** argv) {
	mqd_t mqdes;
	mqdes = mq_open("/test.mq", O_RDONLY);
	struct mq_attr attr;
	if (mq_getattr(mqdes, &attr) == -1){
		perror("mq_getattr");
		exit(1);
	}
	void* buf;
	buf = malloc(attr.mq_msgsize);
	if(buf == NULL){
		perror("malloc");
		exit(1);
	}
	ssize_t nr;
	nr = mq_receive(mqdes, buf, attr.mq_msgsize, NULL);
	if(nr != 0){
		FILE* fp;
		fp = fopen("/home/rostislav/message.txt", "w+");
		fprintf(fp, "%s", (char*)buf);
		fclose(fp);
		return 0;
	}else {
		perror("no recieve bytes");
		return 1;
	}
}
