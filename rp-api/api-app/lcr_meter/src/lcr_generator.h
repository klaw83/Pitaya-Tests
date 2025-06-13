/**
* $Id: $
*
* @brief Red Pitaya application library Impedance Analyzer module interface
*
* @Author Luka Golinar <luka.golinar@gmail.com>
*
* (c) Red Pitaya  http://www.redpitaya.com
*
* This part of code is written in C programming language.
* Please visit http://en.wikipedia.org/wiki/C_(programming_language)
* for more details on the language used herein.
*/

#ifndef __LCRGENERATOR_H
#define __LCRGENERATOR_H
#include <stdint.h>
#include <stdbool.h>
#include <string>
#include <mutex>

#include "rp.h"
#include "lcrApp.h"

class CLCRGenerator{

public:

    CLCRGenerator();
    ~CLCRGenerator();

    CLCRGenerator(CLCRGenerator &) = delete;
    CLCRGenerator(CLCRGenerator &&) = delete;

    auto start() -> int;
    auto stop() -> int;
    auto setDefault() -> void;
    auto setFreq(uint32_t _freq) -> void;
    auto getFreq() -> uint32_t;
    auto setAmplitude(float _ampl) -> void;
    auto getAmplitude() -> float;
    auto setOffset(float _offset) -> void;
    auto getOffset() -> float;

private:
    std::mutex m_mutex;
    uint32_t m_freq= 10;
    float m_amplitude = 0.5;
    float m_offset = 0;
};

#endif //__LCRGENERATOR_H
