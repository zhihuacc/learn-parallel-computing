#include <iostream>
#include <vector>
#include <omp.h>

#include "hpc_helpers.h"

int main() {
  
  const uint64_t num_entries = 1UL << 30;
  TIMERSTART(alloc);
  std::vector<int> x(num_entries);
  std::vector<int> y(num_entries);
  std::vector<int> z(num_entries);
  TIMERSTOP(alloc);

  TIMERSTART(init_seq);
  for (uint64_t i = 0; i < num_entries; i++) {
    if (i % 2 == 0) {
      x[i] = i;
      y[i] = num_entries - i;
    } else {
      x[i] = num_entries - i;
      y[i] = i;
    }
  }
  TIMERSTOP(init_seq);

  TIMERSTART(add_seq);
  for (uint64_t i = 0; i < num_entries; i++) {
    z[i] = x[i] + y[i];
  }
  TIMERSTOP(add_seq);

  TIMERSTART(reset_z);
  z.assign(z.size(), 0);
  TIMERSTOP(reset_z);

  TIMERSTART(init_omp);
  #pragma omp parallel for
  for (uint64_t i = 0; i < num_entries; i++) {
    if (i % 2 == 0) {
      x[i] = i;
      y[i] = num_entries - i;
    } else {
      x[i] = num_entries - i;
      y[i] = i;
    }
  }
  TIMERSTOP(init_omp);

  TIMERSTART(add_omp);
  uint64_t work_todo = num_entries;
  #pragma omp parallel for
  for (uint64_t i = 0; i < work_todo; i++) {
    z[i] = x[i] + y[i];
  }
  TIMERSTOP(add_omp);

  TIMERSTART(check);
  #pragma omp parallel for
  for (uint64_t i = 0; i < num_entries; i++) {
    if (z[i] - num_entries) {
      std::cout << "error at z[" << i << "]=" << z[i] << std::endl;
    }
  }
  TIMERSTOP(check);

}