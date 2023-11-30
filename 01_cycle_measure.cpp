
#include <cstdio>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <intrin.h>

//
// Basic Typedefs
//

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

//
// Random funcs
//

u64 xorshift64(u64 seed = 0) {
  static u64 state = __rdtsc() ^ 0xc8'3e'95'53'6c'a3'9e'4a;
  if (seed != 0) {
    state = seed ^ 0xc8'3e'95'53'6c'a3'9e'4a;
  }
  u64 x = state;

  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;

  state = x;
  return x;
}

u64 rand_u64() {
  return xorshift64();
}

//
// 
//

i32* catalogue;

i32 measure_swaps(i32 deck[], const i32 n) {
  i32 swaps = 0;

  // i32* catalogue = new i32[n];

  for (i32 i = 0; i < n; i++) { catalogue[deck[i]] = i; }

  i32 pilgrim, cycles, temp;

  for (i32 i = 0; i < n; i++) {
    pilgrim = i;
    cycles  = 0;
    while (catalogue[pilgrim] >= 0) {
      temp = pilgrim;
      pilgrim = catalogue[pilgrim];
      catalogue[temp] = -1;
      cycles++;
    }
    if (cycles > 1) {
      swaps += cycles - 1;
    }
  }

  return swaps;
}

void knuth_shuffle(int arr[], int n) {
  for (int i = n - 1; i > 0; i--) {
    int j = rand_u64() % (i + 1);
    int temp = arr[i];
    arr[i]   = arr[j];
    arr[j]   = temp;
  }
}

void print_measurements(f64 measurements[], i32 num_measurements, int n) {
  f64 sum = 0.0, mean, std_dev = 0.0, min = measurements[0], max = measurements[0];
  for (int i = 0; i < num_measurements; i++) {
    sum += measurements[i];
    if (measurements[i] < min) min = measurements[i];
    if (measurements[i] > max) max = measurements[i];
  }

  mean = sum / num_measurements;

  for (int i = 0; i < num_measurements; i++) {
    std_dev += pow(measurements[i] - mean, 2);
  }

  std_dev = sqrt(std_dev / num_measurements);

  printf("%in\t", n);
  printf("(%.2f...%.2f)\t", min, max);
  printf("Mean: %f\t", mean);
  printf("Std Deviation: %f\n", std_dev);
}

int main() {
  i32 max_n = 101;
  i32 n_offset = 0; // ignore first __ elements, to ignore like 0, 1, 2, etc, which can't really be sorted.

  catalogue = new i32[max_n];

  int measurements_n = max_n-n_offset;
  f64* outer_measurements = new f64[measurements_n];

  f64 sample_size = 10'000;
  f64* inner_measurements = new f64[sample_size];

  int* arr = new int[max_n];
  for (i32 n = n_offset; n < max_n; n++) {
    for (int i = 0; i < n; i++) { arr[i] = i; }

    f64 avg_approx_measure = 0.0;
    
    for (i32 sample = 0; sample < sample_size; sample++) {
      knuth_shuffle(arr, n);
      inner_measurements[sample] = measure_swaps(arr, n);
      avg_approx_measure += inner_measurements[sample];
      inner_measurements[sample] = n - inner_measurements[sample];
    }
    avg_approx_measure /= sample_size;

    outer_measurements[n-n_offset] = n-avg_approx_measure;
    print_measurements(inner_measurements, sample_size, n);
    // printf("n %i\tmeasure %f\tdiff %f\tpercent %f\n", n, avg_approx_measure, n-avg_approx_measure, (n-avg_approx_measure)/n);
  }
  delete[] arr;
  delete[] catalogue;

  return 0;
}
