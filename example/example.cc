#include "../cyg_profile.hpp"

namespace example {

std::string example1() {
    return "";
}

void example2(int a) {}
void example3(int a) {}

}
int main() {
    example::example1();
    example::example2(3);
    return 0;
}