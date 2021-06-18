#include "tick.hpp"
#include "network.hpp"



using namespace std;

int main()
{
 curl_global_init(CURL_GLOBAL_SSL );
    std::cout << "\033]0;" << "Vladimir" << "\007";
    std::thread tick(servertick,"B");
    packet_thread("A");

    while(1);
}
