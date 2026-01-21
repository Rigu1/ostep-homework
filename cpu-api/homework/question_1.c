#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int x = 100;

    pid_t rc = fork();

    if (rc < 0) {
        exit(1);
    } else if (rc == 0) {
        x = 200;
        printf("Child x: %d\n", x);
    } else {
        wait(NULL);
        printf("Parent x: %d\n", x);
    }

    return 0;
}
