#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct message {
    long mtype;
    char mtext[80];
} messg;

int main(int argc, char** argv){
	key_t key = ftok("/tmp/msg.temp", 1);
	int msgid;
	msgid = msgget(key, IPC_CREAT | 0666);
	printf("Message queue created with id %d\n", msgid);
	if (msgrcv(msgid, &messg, 80, 1, 0) < 0) {
		perror("msgrcv");
		exit(1);
	}else {
		FILE* fp;
		fp = fopen("/home/rostislav/message.txt", "w+");
		fprintf(fp, "%s", messg.mtext);
		fclose(fp);
	}
	
	return 0;
}
