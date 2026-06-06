#include <iostream>
#include <omp.h>

int main() {
  #pragma omp parallel num_threads(4)
  {
    int i = omp_get_thread_num();
    int n = omp_get_num_threads();
    std::cout << "Hello from thread " << i << " of " << n << std::endl;
  }
}