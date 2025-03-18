#include <iostream>
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <unistd.h>  // for usleep()

#define BME680_I2C_ADDR 0x76  // Change if needed

// Class for BME680 Sensor (Modify based on the real sensor library)
class BME680Sensor {
private:
    int i2c_fd;
public:
    BME680Sensor(int address) {
        i2c_fd = wiringPiI2CSetup(address);
        if (i2c_fd == -1) {
            std::cerr << "Error: Unable to initialize I2C for BME680!" << std::endl;
            exit(1);
        }
    }

    void readData() {
        int temp_raw = wiringPiI2CReadReg16(i2c_fd, 0x22);  // Example register
        float temperature = (temp_raw / 100.0);  // Convert to degrees

        std::cout << "BME680 - Temp: " << temperature << "°C" << std::endl;
    }
};

int main() {
    std::cout << "Initializing Sensors..." << std::endl;
    wiringPiSetup();  // Initialize WiringPi

    BME680Sensor bme680(BME680_I2C_ADDR);

    while (true) {
        bme680.readData();
        usleep(2000000);  // Sleep for 2 seconds
    }

    return 0;
}
