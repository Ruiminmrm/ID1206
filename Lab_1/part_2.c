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
    if(child == 0){//child

        /*creates a new POSIX message queue or opens an existing
        queue.  The queue is identified by name.  For details of the
        construction of name, in this case, "/file_message_queue"
        
        O_RDONLY = receive message only
        */
        mqd_t mqd = mq_open("/file_message_queue", O_RDONLY);
        int num = 0;
        char buffer[1024];
        while(1){
            /*
            ssize_t mq_receive(mqd_t mqdes, char msg_ptr[.msg_len],
                          size_t msg_len, unsigned int *msg_prio);
            */
            int size = mq_receive(mqd, buffer, 1024, 0);
            buffer[size] = 0;
            if(strcmp(buffer, "END") != 0) {//compare until the end of file
                num++;
            }else {
                printf("\n%d\n", num);
                mq_unlink("/file_message_queue");
                exit(0);
            }
        }
    }else{
        /*
        struct mq_attr {
               long mq_flags;        Flags (ignored for mq_open()) 
               long mq_maxmsg;       Max. # of messages on queue 
               long mq_msgsize;      Max. message size (bytes) 
               long mq_curmsgs;      # of messages currently in queue
                                       (ignored for mq_open()) 
           };
        */
        struct mq_attr attributes;
        attributes.mq_flags = 0;
        attributes.mq_maxmsg = 10;
        attributes.mq_msgsize = 1024;
        attributes.mq_curmsgs = 0;
        /*
        O_CREAT = Create the message queue if it does not exist.  The owner
              (user ID) of the message queue is set to the effective
              user ID of the calling process.  The group ownership
              (group ID) is set to the effective group ID of the calling
              process.
        O_WRONLY = send only
        0644 = * (owning) User: read & write
               * Group: read
               * Other: read
             = -rw-r--r--
        */
        mqd_t mqd = mq_open("/file_message_queue", O_CREAT | O_WRONLY, 0644, &attributes);
        FILE* file = fopen("file.txt", "r");
        char buffer[1024];
        while (fscanf(file, "%s", buffer) > 0) {
            mq_send(mqd, buffer, strlen(buffer), 0); //add to the message queue
	    }
        fclose(file);
        strcpy(buffer, "END"); //add a marker to the buffer so it knows where is the end of file.
        mq_send(mqd, buffer, strlen(buffer), 0);
        mq_close(mqd);
    }
    return 0;
}







