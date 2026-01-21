#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("hello world (pid:%d)\n", (int) getpid());
    int rc = fork();

    if (rc < 0) {
        exit(1);
    } else if (rc == 0) {
        int child_wc = waitpid(-1, NULL, 0); 
        printf("Child(pid:%d) waitpid() result: %d\n", (int) getpid(), child_wc);
        printf("child(pid:%d)\n", (int) getpid());
    } else {
        int wc = waitpid(rc, NULL, 0); 
        printf("parent(rc:%d) (wc:%d) (pid:%d)\n", rc, wc, (int) getpid());
    }

    return 0;
}