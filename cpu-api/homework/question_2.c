#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int fd = open("test.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    
    if (fd < 0) {
        exit(1);
    }

    printf("parent 파일 디스크립터(fd) = %d\n", fd);

    pid_t rc = fork();

    if (rc < 0) {
        exit(1);
    } else if (rc == 0) {
        const char *child_msg = "with Child\n";
        write(fd, child_msg, strlen(child_msg));
        printf("child 파일 쓰기 완료 (fd: %d)\n", fd);
    } else {
        wait(NULL);
        const char *parent_msg = "with parent\n";
        write(fd, parent_msg, strlen(parent_msg));
        printf("parent 파일 쓰기 완료 (fd: %d)\n", fd);
        close(fd);
    }

    return 0;
}
