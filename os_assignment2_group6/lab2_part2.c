#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX 5

typedef struct {
    //the lock
    sem_t lock;
    int VAR;
    int count_of_readers;
} BufferControl;

// Reader function: Implements the reader's behavior.
void perform_read(BufferControl* control, int reader_id) {
    while(1) {
        
        if((control->VAR) == MAX){
            break;
        }

        (control->count_of_readers)++;

        if ((control->count_of_readers) <= 1) {
            sem_wait(&control->lock);
            printf("The first reader acquires the lock.\n"); // Print when the first reader acquires the lock
        }

        // Reading the current value of the buffer along with reader's PID.
        printf("The reader (%d) reads the value %d\n", reader_id, control->VAR);

        (control->count_of_readers)--;

        if ((control->count_of_readers) == 0) {
            sem_post(&control->lock);
            printf("The last reader releases the lock.\n"); // Print when the last reader releases the lock
        }
        

        sleep(2); // Delay for visibility.
    }
}


void perform_write(BufferControl* control, int writer_id) {
    while(1){
        sem_wait(&control->lock);
        printf("The writer acquires the lock.\n");

        control->VAR++;
        printf("The writer (%d) writes the value %d\n", writer_id, control->VAR);

        sem_post(&control->lock);
        printf("The writer releases the lock.\n");

        if((control->VAR) == MAX){
            break;
        }

        sleep(2);
    }
}

int main() {
    //create shared memory
    int shm_id = shmget(IPC_PRIVATE, sizeof(BufferControl), IPC_CREAT | 0666);
    BufferControl* control = (BufferControl*)shmat(shm_id, NULL, 0);

    //set default
    control->VAR = 0;
    control->count_of_readers = 0;

    sem_init(&control->lock, 1, 1);

    pid_t pid_r1, pid_r2, pid_w;

    //creating process
    if ((pid_r1 = fork()) == 0) {
        perform_read(control, getpid());
        exit(EXIT_SUCCESS);
    }

    if ((pid_r2 = fork()) == 0) {
        perform_read(control, getpid());
        exit(EXIT_SUCCESS);
    }

    if ((pid_w = fork()) == 0) {
        perform_write(control, getpid());
        exit(EXIT_SUCCESS);
    }

    waitpid(pid_w, NULL, 0);

    //clean up
    sem_destroy(&control->lock);

    shmdt(control);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
