#include <vector>
#include <atomic>
#include <omp.h>

#include "hpc_helpers.h"

int main() {

  // case 1: change boundary condition in for-loop
  {
    LOG("case 1:");
    uint64_t work_todo = 1000000UL;
    std::atomic<uint64_t> safe_work_done{0};
    uint64_t unsafe_workdone = 0;
    #pragma omp parallel for
    for (uint64_t i = 0; i < work_todo; i++) {
      safe_work_done++;
      unsafe_workdone++;
      if (i == 1000) {
        work_todo /= 2;
        LOG("Thread ", omp_get_thread_num(), " changed work_todo");
      }
    }

    LOG("work_todo = ", work_todo);
    LOG("safe_work_done = ", safe_work_done.load());
    LOG("unsafe_workdone = ", unsafe_workdone);
  }

  
  {
    LOG("\ncase 2:");
    const uint64_t num = 20;
    std::vector<int> v(num);
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < num; i++) {

      if (i == 5)
          i += 10;
      v[i] += i;  // v[i>=15] are raced, and may be incremented twice.
    }

    LOG("expected v: 0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 17, 18, 19");
    std::ostringstream _actual;
    _actual << "  actual v: ";
    for (int i = 0; i < num; i++) {
      _actual << v[i] << ", ";
    }
    LOG(_actual.str());
  }

  {
    LOG("\ncase 3:");
    int x = 100;
    LOG("shared(x)");
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < 8; i++) {
      x++;
      LOG("Thread ", omp_get_thread_num(), ", &x = ", std::hex, &x, ", x = ", std::dec, x);
    }

    x = 100;
    LOG("\nprivate(x)");
    #pragma omp parallel for num_threads(4) private(x)
    for (int i = 0; i < 8; i++) {
      x++; // private x is not intialzied, so here will produce garbage.
      LOG("Thread ", omp_get_thread_num(), ", &x = ", std::hex, &x, ", x = ", std::dec, x);
    }

    x = 100;
    LOG("\nfirstprivate(x)");
    #pragma omp parallel for num_threads(4) firstprivate(x)
    for (int i = 0; i < 8; i++) {
      x++; // firstprivate x is initialized with original x.
      LOG("Thread ", omp_get_thread_num(), ", &x = ", std::hex, &x, ", x = ", std::dec, x);
    }
    LOG("after loop, x = ", x);


    LOG("\nlastprivate(x)");
    #pragma omp parallel for num_threads(4) lastprivate(x)
    for (int i = 0; i < 8; i++) {
      x = i; // lastprivate x is uninitialized
      LOG("Thread ", omp_get_thread_num(), ", &x = ", std::hex, &x, ", x = ", std::dec, x);
    }
    LOG("after loop, x = ", x);
  }

}