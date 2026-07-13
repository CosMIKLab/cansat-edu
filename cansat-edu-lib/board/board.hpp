#ifndef BOARD_HPP
#define BOARD_HPP

#include <Arduino.h>
#include <Wire.h>
#include <config.hpp>

class Board {
public:
    void init();
};

extern Board board;

#endif /* BOARD_HPP */
