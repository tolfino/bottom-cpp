#include <iostream>

#include "bottom.hpp"

using namespace bottom;

int main()
{
    std::cout << decode<decltype("💖💖✨🥺,👉👈💖💖,👉👈💖💖✨🥺👉👈💖💖✨🥺,👉👈"_C)>::to_string() << std::endl;
    std::cout << encode<decltype("test"_C)>::to_string() << std::endl;
}
