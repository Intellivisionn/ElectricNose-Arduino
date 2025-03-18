#!/bin/bash

echo "Starting Arduino Compilation and Upload..."

# Change to project directory
cd /home/admin/ElectricNose-Arduino

# Check for network and pull updates
if ping -c 1 github.com &> /dev/null; then
    echo "Network available, pulling latest updates..."
    git pull origin main
else
    echo "No network, using existing version."
fi

# Install Arduino CLI if not installed
if ! command -v arduino-cli &> /dev/null; then
    echo "Installing Arduino CLI..."
    curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
fi

# Update Arduino CLI core index
arduino-cli core update-index

# Install board support for Arduino (adjust if using ESP32 or ESP8266)
arduino-cli core install arduino:avr

# Detect connected Arduino board
BOARD_PORT=$(arduino-cli board list | grep -oE '/dev/tty[^ ]+')

if [ -z "$BOARD_PORT" ]; then
    echo "No Arduino board detected! Please connect one."
    exit 1
fi

# Compile the Arduino Sketch
arduino-cli compile --fqbn arduino:avr:uno arduino_code/my_arduino_code.ino

# Upload the compiled code to the Arduino
arduino-cli upload -p $BOARD_PORT --fqbn arduino:avr:uno arduino_code/my_arduino_code.ino

echo "Arduino code uploaded successfully!"

# (Optional) Start Serial Monitor
arduino-cli monitor -p $BOARD_PORT -c baudrate=9600
