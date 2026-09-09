#!/bin/bash

# Stop if any command fails
set -e

# Move into src directory
cd src || exit 1


# =========================
# Compile main engine
# =========================

echo "Compiling main.cpp..."

g++ -O3 -march=native -flto -fno-plt -fomit-frame-pointer \
    -DNDEBUG main.cpp 

echo "Engine compilation finished."


# =========================
# Compile Calculator
# =========================

echo "Compiling Calculator.cpp..."

g++ Calculator.cpp -o update_opening

echo "Calculator compilation finished."


# =========================
# Create games directory
# =========================

mkdir -p games


# =========================
# Find game number
# =========================

gamenum=$(find ./games -maxdepth 1 -type f -name "game_*.txt" | wc -l)


# =========================
# Run engine
# =========================

echo ""
echo "================================="
echo "Running engine..."
echo "Saving output to games/game_${gamenum}.txt"
echo "================================="
echo ""

# Display output in terminal AND save to file
./a.out 2>&1 | tee "./games/game_${gamenum}.txt"


echo ""
echo "================================="
echo "Game finished."
echo "Output saved to ./games/game_${gamenum}.txt"
echo "================================="