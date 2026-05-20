#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "config.hpp"

class Board {
public:
    void init();
};

extern Board board;
