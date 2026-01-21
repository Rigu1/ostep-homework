# CH 08. Process API

### `fork()`를 호출하는 프로그램을 작성하라

> `fork()`를 호출하기 전에 메인 프로세스는 변수에 접근하고 (예, x) 변수에 값을 지정하라 (예, 100)

```c
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
```

- 자식 프로세스에서 그 변수의 값은 무엇인가? 
    - 200이다.
- 부모와 자식이 변수 x를 변경한 후에 변수는 어떻게 변했는가?
    - 처음에는 100, 자식에서는 200, 자식 프로세스가 종료되고 다시 부모에서는 100이다.


### `open()` 시스템 콜을 사용하여 파일을 여는 프로그램을 작성하고 새 프로세스를 생성하기 위하여 `fork()`를 호출하라

```c
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
```

- 자식과 부모가 `open()`에 의해 반환된 파일 디스크립터에 접근할 수 있는가?
    - 접근할 수 있다.
- 부모와 자식 프로세스가 동시에 파일에 쓰기 작업을 할 수 있는가?
    - 부모와 자식이 동시에 파일 쓰기 작업을 수행할 수 있다.

### `fork()`를 사용하는 다른 프로그램을 작성하라

```c
int main() {
    int rc = fork();

    if (rc < 0) {
        exit(1);
    } else if (rc == 0) {
        printf("hello\n");
    } else {
        int wc = wait(NULL); 
        printf("goodbye\n");
    }

    return 0;
}
```

> 자식 프로세스는 “hello”를 출력하고 부모 프로세스는 “goodbye”를 출력해야 한다. 항상 자식 프로세스가 먼저 출력하게 하라

- 부모가 `wait()`를 호출하지 않고 할 수 있는가?
    - Pipe() - 파이프의 블로킹 특성을 활용
    - Signal() - 시그널을 이용하여 이벤트 동기화
    - 공유 메모리 - 공유 메모리를 통해 모니터링

### `fork()`를 호출하고 /bin/ls를 실행하기 위하여 `exec()` 계열의 함수를 호출하는 프로그램을 작성하라. 

> exec()의 변형 execl(), execle(), execlp()
execv(), execvp(), execve() 모두를 사용할 수 있는지 시도해 보라

```c
int main() {
    pid_t rc = fork();

    if (rc < 0) {
        exit(1);
    } else if (rc == 0) {        
        // 1. execl: 리스트(List) 방식, 전체 경로 필요
        // execl("/bin/ls", "ls", "-l", NULL);

        // 2. execlp: 리스트 방식, PATH 환경변수 참조
        // execlp("ls", "ls", "-l", NULL);

        // 3. execle: 리스트 방식, 환경변수(envp) 직접 전달
        // char *envp[] = { "USER=rigu", NULL };
        // execle("/bin/ls", "ls", "-l", NULL, envp);

        // 4. execv: 벡터(Vector/Array) 방식, 전체 경로 필요
        // char *args[] = {"ls", "-l", NULL};
        // execv("/bin/ls", args);

        // 5. execvp: 벡터 방식, PATH 환경변수 참조
        char *args[] = {"ls", "-l", NULL};
        execvp("ls", args);

        // 6. execve: 벡터 방식, 환경변수 직접 전달 (가장 원시적인 시스템 콜)
        // char *args[] = {"ls", "-l", NULL};
        // char *envp[] = { "USER=rigu", NULL };
        // execve("/bin/ls", args, envp);

        printf("출력되지 않을 문장");
    } else {
        wait(NULL);
        printf("\n부모 프로세스: 자식의 ls 실행 종료\n");
    }
    return 0;
}
```

- 기본적으로는 동일한 기능을 수행하는 시스템 콜에 여러 변형이 있는 이유를 생각해 보라.
    - 상황에 맞는 인터페이스를 제공하기 위해서이다. 기본적으로 다른 변형들은 모두 `execve()`를 호출하기에 사살상 여러 상황에 맞춰 사용할 수 있도록 래핑하여 인터페이스를 제공하고 있는 것이다.
    
        | 구분 | pre | Full Name | 상세 동작 및 제어 범위 |
        | :--- | :---: | :--- | :--- |
        | **인자 전달** | **`l`** | **List** | 함수 인자의 가변 인자 리스트(`va_list`)를 통해 명령행 인자를 하나씩 나열<br>마지막 인자는 반드시 `(char *)0` 또는 `NULL`로 종료되어야 한다 |
        | **인자 전달** | **`v`** | **Vector** | 인자들을 문자열 포인터 배열(`char *const argv[]`)에 담아 배열의 주소를 전달<br>런타임에 인자의 개수가 결정되는 동적 상황에 적합 |
        | **경로 탐색** | **`p`** | **Path** | 실행 파일의 이름만 제공될 경우, 환경변수 `PATH`에 등록된 디렉토리 목록을 순차적으로 탐색하여 절대 경로를 자동으로 식별 |
        | **환경 제어** | **`e`** | **Environment** | 호출 프로세스(부모 프로세스)의 환경변수를 상속받지 않고, 사용자가 정의한 새 환경변수 배열(`char *const envp[]`)을 프로세스 이미지에 직접 주입 |

### `wait()`를 사용하여 자식 프로세스가 종료되기를 기다리는 프로그램을 작성하라

```c
int main() {
    printf("hello world (pid:%d)\n", (int) getpid());
    int rc = fork();

    if (rc < 0) {
        exit(1);
    } else if (rc == 0) {
        printf("Child(pid:%d) Call wait()\n", (int) getpid());
        
        int child_wc = wait(NULL);
        printf("Child(pid:%d) wait() result: %d\n", (int) getpid(), child_wc);
        
        printf("child (pid:%d)\n", (int) getpid());
    } else {
        int wc = wait(NULL);
        printf("parent (rc:%d) (wc:%d) (pid:%d)\n", rc, wc, (int) getpid());
    }

    return 0;
}
```

- `wait()`가 반환하는 것은 무엇인가? 
    - 종료된 자식의 PID다. 에러 발생 시에는 -1을 반환한다. 
- 자식 프로세스가 `wait()`를 호출하면 어떤 결과가 발생하는가?
    - 기다릴 자식 프로세스가 없기에 -1을 반환한다.


### 위 문제에서 작성한 프로그램을 수정하여 `wait()` 대신에 `waitpid()`를 사용하라

```c
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
```

- 어떤 경우에 `waitpid()`를 사용하는 것이 좋은가?
    - 특정 프로세스의 종료를 기다릴 때 사용하는 것이 좋다. 프로세스의 처리 순서를 예측할 수 있다.

### 자식 프로세스를 생성하고 자식 프로세스가 표준 출력 (STDOUT_FILENO)을 닫는 프로그램을 작성하라.

```c
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
        printf("parent 자식의 실행 종료\n");
    }

    return 0;
}
```

- 자식이 설명자를 닫은 후에 아무거나 출력하기 위하여 `printf()`를 호출하면 무슨 일이 생기는가?
    - 자식과 부모의 stdout은 같은 설명자를 참조하고 있으며 자식 프로세스에서 설명자를 닫는다는 것이 stdout이 가리키는 파일 디스크럽터 엔트리를 닫는 것이지 공유 중인 커널의 설명자 객체를 해제하는 것이 아니기에 자식 기존 부모의 stdout은 동일한 설명자를 참조하고 있다. 고로 엔트리가 해제된 자식 프로세스에선 출력이 불가하지만 부모 프로세스에서 `prinf()`를 호출하면 정상적으로 출력이 되는 모습을 확인할 수 있다. 

### 두 개의 자식 프로세스를 생성하고 pipe() 시스템 콜을 사용하여 한 자식의 표준 출력을 다른 자식의 입력으로 연결하는 프로그램을 작성하라.

```c
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
```