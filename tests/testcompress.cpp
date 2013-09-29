#include <compress.hpp>

#include <vector>
#include <iostream>

using iter::compress;

int main(void)
{
    std::vector<int> ivec{1, 2, 3, 4, 5, 6};
    std::vector<bool> bvec{true, false, true, false, true, false};

    for (auto i : compress(ivec, bvec)) {
        std::cout << i << '\n';
    }
    return 0;
}
