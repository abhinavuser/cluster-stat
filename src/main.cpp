#include "main.hpp"

using namespace std;
using namespace socket_service;
using namespace display;

int main(){

    std::thread display_thread (display_init);

    std::thread socket_thread (socket_init);

    while(1){}
    
    return 0;
}
