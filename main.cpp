#include <iostream>
#include <unistd.h>  // For sleep()
#include <cstdlib>   // For rand()

int main() {
    std::cout << "Arduino Simulator Running..." << std::endl;

    while (true) {
        int sensorValue = rand() % 100;  // Simulated sensor data
        std::cout << "SENSOR_DATA: " << sensorValue << std::endl;
        std::cout.flush();
        sleep(1);  // Send data every second
    }

    return 0;
}
