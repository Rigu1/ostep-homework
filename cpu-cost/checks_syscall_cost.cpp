#include <iostream>
#include <unistd.h>
#include <chrono>
#include <fcntl.h>

void measure_syscall_cost(int iterations) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) {
        std::cerr << "File open error!" << std::endl;
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        read(fd, NULL, 0); //시스템 콜 트랩만 발생
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> elapsed = end - start;

    std::cout << "========================================" << std::endl;
    std::cout << "[System Call Cost Analysis]" << std::endl;
    std::cout << "Total iterations : " << iterations << std::endl;
    std::cout << "Average cost     : " << elapsed.count() / iterations << " ns" << std::endl;
    std::cout << "========================================" << std::endl;
    close(fd);
}

int main() {
    measure_syscall_cost(1'000'000);
    return 0;
}