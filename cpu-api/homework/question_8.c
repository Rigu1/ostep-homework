#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t p1 = fork();

    if (p1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);

        printf("Child 1에서 전송된 데이터\n");
        fflush(stdout);
        exit(0);
    }

    pid_t p2 = fork();

    if (p2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        
        char buf[1024];
        if (fgets(buf, sizeof(buf), stdin) != NULL) {
            printf("\n[Chile 2가 받은 데이터]: %s\n", buf);
        }
        exit(0);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    wait(NULL);
    wait(NULL);

    return 0;
}