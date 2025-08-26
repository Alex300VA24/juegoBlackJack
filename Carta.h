// Carta.h
#pragma once
#include <string>

// Estrutura que representa uma carta na lógica do jogo (sem parte gráfica)
struct Carta {
    std::string valor;  // Valor da carta: "A", "2", ..., "K"
    std::string naipe;  // Naipe da carta: "hearts", "spades", "diamonds", "clubs"
    int peso;           // Peso numérico usado para pontuação (1 a 11, dependendo do valor)
};
