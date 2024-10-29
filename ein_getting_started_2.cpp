#include <vector>
#include <iostream>
#include <cassert>

import ein.simd;

void simd_add_assign(
  std::vector<float>& xs,
  std::vector<float> const& ys) {

  std::size_t size = xs.size();
  std::size_t chunks = size / 4;

  float* p_x = xs.data();
  float const* p_y = ys.data();

  for (std::size_t i = 0; i < chunks; ++i) {
    ein::simd<float, 4> x = ein::load<4>(p_x + i*4);
    ein::simd<float, 4> y = ein::load<4>(p_y + i*4);

    x += y;

    ein::store(p_x + i * 4, x);
  }

  for (std::size_t i = 4 * chunks; i < size; ++i) {
    p_x[i] += p_y[i];
  }

}

int main() {
  std::vector<float> xs{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  std::vector<float> ys{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

  assert(reinterpret_cast<uintptr_t>(xs.data()) % 16 != 0);
  assert(reinterpret_cast<uintptr_t>(ys.data()) % 16 != 0);

  simd_add_assign(xs, ys);

  for (auto x: xs) {
    std::cout << x << std::endl;
  }

  return 0;
}
