#include <iostream>
#include <unistd.h>  // For sleep()

int main() {
    std::cout << "Arduino Simulator Running..." << std::endl;
    
    while (true) {
        std::cout << "Heartbeat" << std::endl;  // Just a visibility signal
        std::cout.flush();
        sleep(5);  // Delay of 5 seconds
    }

    return 0;
}
