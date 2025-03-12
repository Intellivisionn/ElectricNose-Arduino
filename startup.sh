#!/bin/bash

echo "Starting Arduino Simulation..."

# Change to project directory
cd /home/admin/ElectricNose-Arduino

# Try to pull updates (only if network is available)
if ping -c 1 github.com &> /dev/null; then
    echo "Network available, pulling latest updates..."
    git pull origin main
else
    echo "No network, using existing version."
fi

# Compile the latest code
g++ -o arduino_sim main.cpp

# Run the Arduino Simulation
./arduino_sim
