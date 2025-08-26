// Jogo.h
#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include "Carta.h"

// Classe que representa o baralho e as operações básicas do jogo Blackjack
class Jogo {
private:
    std::vector<Carta> baralho; // Vetor que armazena todas as cartas do baralho
    int indiceAtual = 0;        // Índice da próxima carta a ser retirada

public:
    // Construtor: cria o baralho e o embaralha automaticamente
    Jogo() {
        criarBaralho();
        embaralhar();
    }

    // Função que cria o baralho de 52 cartas (13 valores × 4 naipes)
    void criarBaralho() {
        std::string valores[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
        std::string naipes[] = {"spades", "hearts", "diamonds", "clubs"};

        // Para cada naipe e valor, cria uma carta com peso associado
        for (const auto& naipe : naipes) {
            for (const auto& valor : valores) {
                int peso = 0;
                if (valor == "A") peso = 11;                        // Ás começa valendo 11
                else if (valor == "J" || valor == "Q" || valor == "K") peso = 10; // Figuras valem 10
                else peso = std::stoi(valor);                       // Números mantêm seu valor

                // Adiciona carta ao baralho
                baralho.push_back({valor, naipe, peso});
            }
        }
    }

    // Embaralha o baralho usando gerador de números aleatórios
    void embaralhar() {
        std::random_device rd;           // Fonte de entropia
        std::mt19937 g(rd());            // Gerador de números pseudoaleatórios
        std::shuffle(baralho.begin(), baralho.end(), g); // Embaralha as cartas
        indiceAtual = 0;                 // Reinicia o índice para o início
    }

    // Retira uma carta do baralho
    Carta tirarCarta() {
        if (indiceAtual < baralho.size()) {
            // Retorna a carta atual e avança o índice
            return baralho[indiceAtual++];
        } else {
            // Se acabar o baralho, embaralha novamente e retira
            embaralhar();
            return tirarCarta();
        }
    }
};
