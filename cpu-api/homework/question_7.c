#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t rc = fork();

    if (rc < 0) {
        return 1;
    } else if (rc == 0) {
        printf("Close stdout\n");

        close(STDOUT_FILENO);

        printf("출력되지 않는 문장");
    } else {
        wait(NULL);
        printf("parent 자식의 실행이 끝났습니다.\n");
    }

    return 0;
}