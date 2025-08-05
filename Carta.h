// Carta.h
#pragma once
#include <string>

struct Carta {
    std::string valor;  // "A", "2", ..., "K"
    std::string naipe;  // "hearts", "spades", etc.
    int peso;           // 1–11 según el valor
};
