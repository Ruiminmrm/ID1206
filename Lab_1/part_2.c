#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>
/*In this part, you will work with message queues. You need to implement two
processes, such that the first process reads the content of a text file and passes it
to the second process through a message queue. Upon receipt of the file content,
the second process should count and print out the number of words in the file.*/

int main(){
    pid_t child = fork();
    if(child == 0){
        mqd_t mqd = mq_open("/file_message_queue", O_RDONLY);
        int num = 0;
        char buffer[1024];
        while(1){
            int size = mq_receive(mqd, buffer, 1024, 0);
            buffer[size] = 0;
            if(strcmp(buffer, "FIN") != 0) {
                num++;
            }else {
                printf("\n%d\n", num);
                mq_unlink("/file_message_queue");
                exit(0);
            }
        }
    }else{
        struct mq_attr attributes;
        attributes.mq_flags = 0;
        attributes.mq_maxmsg = 10;
        attributes.mq_msgsize = 1024;
        attributes.mq_curmsgs = 0;
        mqd_t mqd = mq_open("/file_message_queue", O_CREAT | O_WRONLY, 0644, &attributes);
        FILE* file = fopen("file.txt", "r");
        char buffer[1024];
        while (fscanf(file, "%s", buffer) > 0) {
            mq_send(mqd, buffer, strlen(buffer), 0);
	    }
        fclose(file);
        strcpy(buffer, "FIN");
        mq_send(mqd, buffer, strlen(buffer), 0);
        mq_close(mqd);
    }
    return 0;
}







