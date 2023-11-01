#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
/*
The purpose of this part is to work with fork, exec, and wait to create new
processes and use pipe to communicate between parent/child processes. You
should implement a code to illustrate the following command: ls / | wc -l.
This command prints out the number of files in the root path: ls / shows the
files/directories in the root path, and its output will be piped through | to wc
-l, which counts the number of lines.
Hint1: Use fork to make a child process. Then, the child process executes
ls /, passing the result (i.e., the list files/directories) through a pipe to the
parent process. The parent executes wc -l to print out the number of lines for
the list passed by the child.
Hint2: You can use dup2 to redirect the output of the exec to the input descriptor made by pipe.
*/

void main() {

    //An array to store info
    int fd[2];   
    int status, done = 0;
    //returns 2 open file descriptors in the array
    //[0] for read end and [1] for write end
    pipe(fd);

    switch(fork()){
        case 0: // child
            //the file descriptor no longer refers to any file
            close(fd[0]);
            //creates a copy of a file descriptor
            dup2(fd[1], 1);
            close(fd[1]);
            //excuting the ls / command
            execlp("ls","ls","/", (char *) NULL);
        default: //parent
            close(fd[1]);
            dup2(fd[0], 0);
            close(fd[0]);
            execlp("wc", "wc", "-l", (char *) NULL);
    }    

    close(fd[0]);
    close(fd[1]);
    waitpid(-1, &status, 0);
}
