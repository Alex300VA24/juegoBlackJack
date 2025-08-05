// Jogo.h
#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include "Carta.h"




class Jogo {
private:
    std::vector<Carta> baralho;
    int indiceAtual = 0;

public:
    Jogo() {
        criarBaralho();
        embaralhar();
    }

    void criarBaralho() {
        std::string valores[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
        std::string naipes[] = {"spades", "hearts", "diamonds", "clubs"};

        for (const auto& naipe : naipes) {
            for (const auto& valor : valores) {
                int peso = 0;
                if (valor == "A") peso = 11;
                else if (valor == "J" || valor == "Q" || valor == "K") peso = 10;
                else peso = std::stoi(valor);

                baralho.push_back({valor, naipe, peso});
            }
        }
    }

    void embaralhar() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(baralho.begin(), baralho.end(), g);
        indiceAtual = 0;
    }

    Carta tirarCarta() {
        if (indiceAtual < baralho.size()) {
            return baralho[indiceAtual++];
        } else {
            embaralhar();
            return tirarCarta();
        }
    }
};
