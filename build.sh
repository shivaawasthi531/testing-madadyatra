#!/usr/bin/env bash
set -e

echo "Building C++ files..."

cd backend/cpp

g++ -std=c++17 main.cpp -o bus_engine
g++ -std=c++17 metro_engine.cpp -o metro_engine

chmod +x bus_engine metro_engine

echo "Build complete"
