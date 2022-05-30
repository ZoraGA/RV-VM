#include <stdio.h>
#include <stdint.h>

void test_fn1(uint32_t i)
{
    // for (size_t j=0; i<5; i++) {
    //     j += i;
    // }
    i += 1024;
}

int main(int argc, char **argv)
{
    test_fn1(0);
    return 0;
}