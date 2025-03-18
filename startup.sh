#!/bin/bash

echo "Starting Arduino Simulation on Raspberry Pi..."

# Change to project directory
cd /home/admin/ElectricNose-Arduino

# Check for network and pull updates
if ping -c 1 github.com &> /dev/null; then
    echo "Network available, pulling latest updates..."
    git pull origin main
else
    echo "No network, using existing version."
fi

# Build WiringPi from the local repository
if [ ! -f "/usr/local/bin/gpio" ]; then
    echo "Installing WiringPi from local files..."
    cd libraries/WiringPi
    ./build
    cd ../..
fi

# Ensure I2C is enabled
sudo raspi-config nonint do_i2c 0

# Compile the Arduino Simulation Code with WiringPi
echo "Compiling simulation..."
g++ -o sensor_sim main.cpp -I./libraries/WiringPi -L./libraries/WiringPi -lwiringPi -li2c

# Run the simulation
echo "Running simulation..."
./sensor_sim
