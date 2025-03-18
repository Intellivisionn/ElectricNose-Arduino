#include <wiringPiI2C.h>
#include <iostream>

#define TCA9548A_ADDRESS 0x70  // I2C Multiplexer Address
#define BME680_ADDRESS 0x76     // BME680 Sensor Address
#define SGP30_ADDRESS 0x58      // SGP30 Sensor Address

// Function to activate a specific multiplexer channel
void selectTCAChannel(int multiplexer_fd, uint8_t channel) {
    wiringPiI2CWrite(multiplexer_fd, 1 << channel);
}

int main() {
    // Initialize multiplexer
    int multiplexer_fd = wiringPiI2CSetup(TCA9548A_ADDRESS);
    if (multiplexer_fd == -1) {
        std::cerr << "Error: Failed to initialize I2C multiplexer!" << std::endl;
        return 1;
    }

    // 🔹 Select Channel 1 for BME680
    selectTCAChannel(multiplexer_fd, 1);
    int bme_fd = wiringPiI2CSetup(BME680_ADDRESS);
    if (bme_fd == -1) {
        std::cerr << "Error: BME680 not detected on Channel 1!" << std::endl;
    } else {
        std::cout << "✅ BME680 detected on Channel 1!" << std::endl;
        int temp_raw = wiringPiI2CReadReg16(bme_fd, 0x1D);  // Example register
        float temperature = temp_raw / 100.0;
        std::cout << "🌡️  Temperature: " << temperature << "°C" << std::endl;
    }

    // 🔹 Select Channel 2 for SGP30
    selectTCAChannel(multiplexer_fd, 2);
    int sgp_fd = wiringPiI2CSetup(SGP30_ADDRESS);
    if (sgp_fd == -1) {
        std::cerr << "Error: SGP30 not detected on Channel 2!" << std::endl;
    } else {
        std::cout << "✅ SGP30 detected on Channel 2!" << std::endl;
        int tvoc = wiringPiI2CReadReg16(sgp_fd, 0x00);  // Example register
        int eco2 = wiringPiI2CReadReg16(sgp_fd, 0x01);
        std::cout << "🛑 Air Quality - TVOC: " << tvoc << " ppb, eCO2: " << eco2 << " ppm" << std::endl;
    }

    return 0;
}
