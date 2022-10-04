#include <stdint.h> // Defines integer types of a particular size

// PART 1

// int array[1000];
// int8_t array[1000];
// int16_t array[1000];
// int32_t array[1000];
// int64_t array[1000];

// int main() {
//   return array[0];
// }

// PART 2

// int array[1000];

int main() {
  // int array[1000];
  // static int array[1000];
  // const int array[1000];
  int * array = new int[1000];
  return array[0];
}