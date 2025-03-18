#include <iostream>
#include <unistd.h>       // For sleep()
#include <wiringPiI2C.h>  // For I2C communication
#include <wiringPi.h>     // For GPIO handling
#include <vector>

#define TCA_ADDRESS 0x70  // TCA9548A Multiplexer I2C address

// Function to select a channel on the TCA9548A multiplexer
void selectTCAChannel(int fd, uint8_t channel) {
    wiringPiI2CWrite(fd, 1 << channel);
}

// Sensor Interface
class SensorInterface {
public:
    virtual void begin(int multiplexer_fd) = 0;
    virtual void readData() = 0;
};

// BME680 Sensor Class
class BME680Sensor : public SensorInterface {
private:
    int fd;
    uint8_t i2cAddress;
    uint8_t channel;
public:
    BME680Sensor(uint8_t address = 0x76, uint8_t ch = 1) {
        i2cAddress = address;
        channel = ch;
    }

    void begin(int multiplexer_fd) override {
        selectTCAChannel(multiplexer_fd, channel);
        fd = wiringPiI2CSetup(i2cAddress);
        if (fd == -1) {
            std::cerr << "Error initializing BME680 sensor!" << std::endl;
            exit(1);
        }
        std::cout << "BME680 initialized on Channel " << (int)channel << "." << std::endl;

        // Initialize BME680 for data readings
        wiringPiI2CWriteReg8(fd, 0x74, 0x89);  // Control humidity register
        wiringPiI2CWriteReg8(fd, 0x72, 0x01);  // Control pressure register
        wiringPiI2CWriteReg8(fd, 0x71, 0x10);  // Control gas register
        usleep(100000);
    }

    void readData() override {
        selectTCAChannel(fd, channel);
        usleep(100000);

        int temp_raw = wiringPiI2CReadReg16(fd, 0x22);
        int humidity_raw = wiringPiI2CReadReg16(fd, 0x25);
        int pressure_raw = wiringPiI2CReadReg16(fd, 0x28);
        int gas_resistance_raw = wiringPiI2CReadReg16(fd, 0x2B);

        float temperature = temp_raw / 100.0;
        float humidity = humidity_raw / 100.0;
        float pressure = pressure_raw / 100.0;
        float gas_resistance = gas_resistance_raw / 1000.0;

        std::cout << "BME680 - Temp: " << temperature << "°C, Humidity: " << humidity
                  << "%, Pressure: " << pressure << " hPa, Gas Resistance: "
                  << gas_resistance << " kOhms" << std::endl;
    }
};

// SGP30 Sensor Class
class SGP30Sensor : public SensorInterface {
private:
    int fd;
    uint8_t i2cAddress;
    uint8_t channel;
public:
    SGP30Sensor(uint8_t address = 0x58, uint8_t ch = 2) {
        i2cAddress = address;
        channel = ch;
    }

    void begin(int multiplexer_fd) override {
        selectTCAChannel(multiplexer_fd, channel);
        fd = wiringPiI2CSetup(i2cAddress);
        if (fd == -1) {
            std::cerr << "Error initializing SGP30 sensor!" << std::endl;
            exit(1);
        }
        std::cout << "SGP30 initialized on Channel " << (int)channel << "." << std::endl;

        // Initialize SGP30 for proper readings
        wiringPiI2CWriteReg8(fd, 0x20, 0x03);
        usleep(200000);
        wiringPiI2CWriteReg8(fd, 0x20, 0x08);
        usleep(100000);
    }

    void readData() override {
        selectTCAChannel(fd, channel);
        usleep(100000);

        wiringPiI2CWrite(fd, 0x20);
        usleep(100000);
        int tvoc = wiringPiI2CReadReg16(fd, 0x21);
        int eco2 = wiringPiI2CReadReg16(fd, 0x22);

        std::cout << "SGP30 - TVOC: " << tvoc << " ppb, eCO2: " << eco2 << " ppm" << std::endl;
    }
};

int main() {
    std::cout << "Initializing I2C sensors..." << std::endl;
    wiringPiSetup();
    int multiplexer_fd = wiringPiI2CSetup(TCA_ADDRESS);

    if (multiplexer_fd == -1) {
        std::cerr << "Error: Failed to initialize I2C multiplexer!" << std::endl;
        return 1;
    }

    BME680Sensor bme680(0x76, 1);
    SGP30Sensor sgp30(0x58, 2);
    std::vector<SensorInterface*> sensors = {&bme680, &sgp30};

    for (auto& sensor : sensors) {
        sensor->begin(multiplexer_fd);
    }

    while (true) {
        std::cout << "\nReading sensor data..." << std::endl;
        for (auto& sensor : sensors) {
            sensor->readData();
        }
        sleep(2);
    }

    return 0;
}
