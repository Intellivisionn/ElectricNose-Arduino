#include "libraries/Adafruit_BME680/Adafruit_BME680.h"
#include "libraries/Adafruit_SGP30/Adafruit_SGP30.h"
#include "libraries/WiringPi/WiringPi.h"  // If needed
#include <Wire.h>


#define TCA_ADDRESS 0x70  // TCA9548A Multiplexer I2C address

// Function to select channel on TCA9548A
void selectTCAChannel(uint8_t channel) {
    Wire.beginTransmission(TCA_ADDRESS);
    Wire.write(1 << channel);
    Wire.endTransmission();
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
    Adafruit_BME680 bme;
    TwoWire* wire;
    uint8_t i2cAddress;
public:
    BME680Sensor(TwoWire* wireInstance, uint8_t address = 0x76) {
        this->wire = wireInstance;
        this->i2cAddress = address;
    }

    void begin() override {
        selectTCAChannel(1);  // Select BME680 on TCA Multiplexer
        if (!bme.begin(i2cAddress, wire)) {
            Serial.println("BME680 not found, check wiring!");
            while (1);
        }
        Serial.println("BME680 initialized!");

        // Configure BME680
        bme.setTemperatureOversampling(BME680_OS_8X);
        bme.setHumidityOversampling(BME680_OS_2X);
        bme.setPressureOversampling(BME680_OS_4X);
        bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme.setGasHeater(320, 150); // 320°C for 150ms
    }

    void readData() override {
        selectTCAChannel(1);  // Selec:\Users\jakub\Desktop\ElectricNose-Arduino\arduino_simct BME680 on TCA Multiplexer
        if (!bme.performReading()) {
            Serial.println("Failed to read from BME680!");
            return;
        }
        Serial.print("BME680 - Temp: ");
        Serial.print(bme.temperature);
        Serial.print("°C, Humidity: ");
        Serial.print(bme.humidity);
        Serial.print("%, Pressure: ");
        Serial.print(bme.pressure / 100.0);
        Serial.print(" hPa, Gas Resistance: ");
        Serial.print(bme.gas_resistance / 1000.0);
        Serial.println(" kOhms");

        // Send data to Raspberry Pi for ML Processing
        Serial.print("DATA,");
        Serial.print(bme.gas_resistance);
        Serial.print(",");
    }
};

// SGP30 Sensor Class
class SGP30Sensor : public SensorInterface {
private:
    Adafruit_SGP30 sgp;
    TwoWire* wire;
public:
    SGP30Sensor(TwoWire* wireInstance) {
        this->wire = wireInstance;
    }

    void begin() override {
        selectTCAChannel(2);  // Select SGP30 on TCA Multiplexer
        if (!sgp.begin(*wire)) {
            Serial.println("SGP30 not found, check wiring!");
            while (1);
        }
        Serial.println("SGP30 initialized!");

        // Set IAQ Baseline (from Python script)
        sgp.setIAQBaseline(0x8973, 0x8AAE);

        // Set Humidity Compensation (from Python script)
        uint16_t humidity_comp = sgp.getHumidityCompensation(22.1, 44.0);
        sgp.setHumidityCompensation(humidity_comp);
    }

    void readData() override {
        selectTCAChannel(2);  // Select SGP30 on TCA Multiplexer
        if (!sgp.IAQmeasure()) {
            Serial.println("Failed to read from SGP30!");
            return;
        }
        Serial.print("SGP30 - TVOC: ");
        Serial.print(sgp.TVOC);
        Serial.print(" ppb, eCO2: ");
        Serial.print(sgp.eCO2);
        Serial.println(" ppm");

        // Send data to Raspberry Pi for ML Processing
        Serial.print(sgp.eCO2);
        Serial.print(",");
        Serial.print(sgp.TVOC);
        Serial.println();
    }
};

// Grove Multichannel Gas Sensor Class
class GroveGasSensor : public SensorInterface {
private:
    GAS_GMXXX<2> gas;
    TwoWire* wire;
    uint8_t i2cAddress;
public:
    GroveGasSensor(TwoWire* wireInstance, uint8_t address = 0x08) {
        this->wire = wireInstance;
        this->i2cAddress = address;
    }

    void begin() override {
        gas.begin(*wire, i2cAddress);
        Serial.println("Grove Multichannel Gas Sensor initialized!");
    }

    void readData() override {
        Serial.print("Grove Gas Sensor - CO: ");
        Serial.print(gas.getGM102B());
        Serial.print(" ppm, NO2: ");
        Serial.print(gas.getGM302B());
        Serial.print(" ppm, NH3: ");
        Serial.print(gas.getGM502B());
        Serial.print(" ppm, CH4: ");
        Serial.print(gas.getGM702B());
        Serial.println(" ppm");
    }
};

// Create sensor objects with configurable I2C parameters
BME680Sensor bme680(&Wire, 0x76);
SGP30Sensor sgp30(&Wire);
GroveGasSensor groveGas(&Wire, 0x08);

void setup() {
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Initializing I2C sensors...");
    Wire.begin();
    /*
    void startSensors()
    for(int i=0; i<3;i++)
    {
      SesorInterface.begin();
    }
    */
    // Initialize sensors
    
    bme680.begin();
    sgp30.begin();
    //groveGas.begin();
}

void loop() {
    Serial.println("\nReading sensor data...");

    // Read and send sensor data to Raspberry Pi
    bme680.readData();
    sgp30.readData();
    //groveGas.readData();

    delay(2000);
}


/*
This code can be used in Python, to directly read the data sent from here:

import serial
ser = serial.Serial('/dev/ttyUSB0', 115200)
while True:
    data = ser.readline().decode().strip()
    print("Received:", data)


*/