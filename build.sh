#!/usr/bin/env bash
g++ backend/cpp/main.cpp -o backend/cpp/bus_engine
g++ backend/cpp/metro_engine.cpp -o backend/cpp/metro_engine
chmod +x backend/cpp/bus_engine backend/cpp/metro_engine
