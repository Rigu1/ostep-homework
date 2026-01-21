#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t rc = fork();

    if (rc < 0) {
        exit(1);
    } else if (rc == 0) {        
        // 1. execl: 리스트(List) 방식, 전체 경로 필요
        execl("ls", "ls", "-l", NULL);

        // 2. execlp: 리스트 방식, PATH 환경변수 참조
        // execlp("ls", "ls", "-l", NULL);

        // 3. execle: 리스트 방식, 환경변수(envp) 직접 전달
        // char *envp[] = { "USER=rigu", NULL };
        // execle("/bin/ls", "ls", "-l", NULL, envp);

        // 4. execv: 벡터(Vector/Array) 방식, 전체 경로 필요
        // char *args[] = {"ls", "-l", NULL};
        // execv("/bin/ls", args);

        // 5. execvp: 벡터 방식, PATH 환경변수 참조
        // char *args[] = {"ls", "-l", NULL};
        // execvp("ls", args);

        // 6. execve: 벡터 방식, 환경변수 직접 전달 (가장 원시적인 시스템 콜)
        // char *args[] = {"ls", "-l", NULL};
        // char *envp[] = { "USER=rigu", NULL };
        // execve("/bin/ls", args, envp);

        printf("출력되지 않을 문장");
    } else {
        wait(NULL);
        printf("\nParent 자식의 ls 실행 종료\n");
    }
    return 0;
}
