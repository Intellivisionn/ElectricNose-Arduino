#include <iostream>
#include <unistd.h>       // For sleep()
#include <wiringPiI2C.h>  // For I2C communication
#include <wiringPi.h>     // For GPIO handling
#include <fstream>        // For logging data
#include <vector>

#define TCA_ADDRESS 0x70  // TCA9548A Multiplexer I2C address

// Function to select channel on TCA9548A
void selectTCAChannel(int fd, uint8_t channel) {
    wiringPiI2CWrite(fd, 1 << channel);
}

// Sensor Interface
class SensorInterface {
public:
    virtual void begin() = 0;
    virtual void readData() = 0;
};

// BME680 Sensor Class
class BME680Sensor : public SensorInterface {
private:
    int fd;  // File descriptor for I2C
    uint8_t i2cAddress;
public:
    BME680Sensor(uint8_t address = 0x76) {
        i2cAddress = address;
    }

    void begin() override {
        fd = wiringPiI2CSetup(i2cAddress);
        if (fd == -1) {
            std::cerr << "Error initializing BME680 sensor!" << std::endl;
            exit(1);
        }
        std::cout << "BME680 initialized!" << std::endl;
    }

    void readData() override {
        int temp_raw = wiringPiI2CReadReg16(fd, 0x22);  // Mock temperature register
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
public:
    SGP30Sensor(uint8_t address = 0x58) {
        i2cAddress = address;
    }

    void begin() override {
        fd = wiringPiI2CSetup(i2cAddress);
        if (fd == -1) {
            std::cerr << "Error initializing SGP30 sensor!" << std::endl;
            exit(1);
        }
        std::cout << "SGP30 initialized!" << std::endl;
    }

    void readData() override {
        int tvoc = wiringPiI2CReadReg16(fd, 0x20);
        int eco2 = wiringPiI2CReadReg16(fd, 0x21);

        std::cout << "SGP30 - TVOC: " << tvoc << " ppb, eCO2: " << eco2 << " ppm" << std::endl;
    }
};

// Grove Multichannel Gas Sensor Class
class GroveGasSensor : public SensorInterface {
private:
    int fd;
    uint8_t i2cAddress;
public:
    GroveGasSensor(uint8_t address = 0x08) {
        i2cAddress = address;
    }

    void begin() override {
        fd = wiringPiI2CSetup(i2cAddress);
        if (fd == -1) {
            std::cerr << "Error initializing Grove Gas Sensor!" << std::endl;
            exit(1);
        }
        std::cout << "Grove Multichannel Gas Sensor initialized!" << std::endl;
    }

    void readData() override {
        int co = wiringPiI2CReadReg16(fd, 0x10);
        int no2 = wiringPiI2CReadReg16(fd, 0x11);
        int nh3 = wiringPiI2CReadReg16(fd, 0x12);
        int ch4 = wiringPiI2CReadReg16(fd, 0x13);

        std::cout << "Grove Gas Sensor - CO: " << co << " ppm, NO2: " << no2 
                  << " ppm, NH3: " << nh3 << " ppm, CH4: " << ch4 << " ppm" << std::endl;
    }
};

// Create sensor objects
BME680Sensor bme680;
SGP30Sensor sgp30;
GroveGasSensor groveGas;

// Function to initialize all sensors
void initializeSensors(std::vector<SensorInterface*>& sensors) {
    for (auto& sensor : sensors) {
        sensor->begin();
    }
}

int main() {
    std::cout << "Initializing I2C sensors..." << std::endl;
    wiringPiSetup();

    std::vector<SensorInterface*> sensors = {&bme680, &sgp30}; //, &groveGas

    initializeSensors(sensors);

    while (true) {
        std::cout << "\nReading sensor data..." << std::endl;
        for (auto& sensor : sensors) {
            sensor->readData();
        }
        sleep(2);
    }

    return 0;
}
