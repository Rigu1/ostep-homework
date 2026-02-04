#include <iostream>
#include <unistd.h>
#include <chrono>
#include <sys/wait.h>

void measure_context_switch_cost(int iterations) {
    int pipe1[2], pipe2[2];
    
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        std::perror("Pipe creation failed");
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        std::perror("Fork failed");
        return;
    } 
    else if (pid == 0) {
        char buf;
        for (int i = 0; i < iterations; ++i) {
            read(pipe1[0], &buf, 1);
            write(pipe2[1], "1", 1);
        }
        exit(0);
    } 
    else {
        char buf;
        
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < iterations; ++i) {
            write(pipe1[1], "1", 1);
            read(pipe2[0], &buf, 1);
        }

        auto end = std::chrono::high_resolution_clock::now();
        wait(NULL);

        std::chrono::duration<double, std::nano> elapsed = end - start;

        std::cout << "========================================" << std::endl;
        std::cout << "[Context Switch Cost Analysis]" << std::endl;
        std::cout << "Total Iterations : " << iterations << std::endl;
        std::cout << "Total Time       : " << elapsed.count() << " ns" << std::endl;
        std::cout << "Average Cost     : " << elapsed.count() / (iterations * 2) << " ns" << std::endl;
        std::cout << "========================================" << std::endl;
    }
}

int main() {
    measure_context_switch_cost(100'000);
    return 0;
}