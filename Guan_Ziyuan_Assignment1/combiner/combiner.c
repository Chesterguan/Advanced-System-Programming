//
// Created by chester on 1/20/18.
//

#include <stdio.h>
#include<unistd.h>
#include<string.h>
#include <stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
//#include "tlpi_hdr.h"
int main() {
    int pfd[2];// we will create two different children to write and read separately
    if (pipe(pfd) == -1) {
        perror("pipe is wrong");
    }
    switch (fork()) {
        case -1:
            perror("The first Child");
        case 0: /*First child is created*/
            if (close(pfd[0] == -1)) {
                perror("Child 1 close");
            }
            /*Duplicate stdout on write end of pipe; close the duplicated file descriptor*/
            if (pfd[1] != STDOUT_FILENO) {   /*Defensive check*/
                if (dup2(pfd[1], STDOUT_FILENO) == -1) {
                    perror("dup2 child 1");
                }
                if (close(pfd[1]) == -1) {
                    perror("close child 1 write");
                }
            }
            system("gcc -o mapper ./mapper.c");
	    execl("mapper","./mapper",NULL);
	    perror("exec mapper");	
        default:
            break; //do nothing
    }
    if(wait(NULL)==-1){  // we need to wait for the first child to finish or the child 2 will mistake
         perror("wait child 1");
    }
    switch (fork()) { /*Create second child*/
        case -1:
            perror("The second Child");
        case 0:
            if (close(pfd[1]) == -1) {
                perror("child 2 close");
            }
            /*Duplicate the stdin on read end of the pipe*/
            if (pfd[0] != STDIN_FILENO) {
                if (dup2(pfd[0], STDIN_FILENO) == -1) {
                    perror("dup2 child 2");
                }
                if (close(pfd[0]) == -1) {
                    perror("close child 2 read");
                }
            }
            system("cc -o ./reducer ./reducer.c");
            execl("reducer","./reducer",NULL);
            execl("reducer","./reducer",NULL);
        default:
            break;
    }
    /*parent close*/
    if(close(pfd[0])==-1){
        perror("close parent read");
    }
    if(close(pfd[1])==-1){
        perror("close parent write");
    }
    if(wait(NULL)==-1){
        perror("Wait child 2");
    }
    exit(EXIT_SUCCESS);
}
