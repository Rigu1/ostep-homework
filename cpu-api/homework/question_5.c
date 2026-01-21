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
        printf("Child(pid:%d) Call wait()\n", (int) getpid());
        
        int child_wc = wait(NULL);
        printf("Child(pid:%d) wait() result: %d\n", (int) getpid(), child_wc);
        
        printf("child(pid:%d)\n", (int) getpid());
    } else {
        int wc = wait(NULL);
        printf("parent(rc:%d) (wc:%d) (pid:%d)\n", rc, wc, (int) getpid());
    }

    return 0;
}
