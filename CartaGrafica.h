// CartaGrafica.h
#ifndef CARTAGRAFICA_H
#define CARTAGRAFICA_H

#include <SFML/Graphics.hpp>
#include <string>

// Classe que representa a parte visual de uma carta
class CartaGrafica {
private:
    sf::Texture textura;        // Textura carregada da imagem da carta
    sf::Sprite sprite;          // Sprite que será desenhado na tela
    std::string nombreArchivo;  // Caminho/arquivo da imagem

public:
    // Construtor: carrega a imagem e posiciona a carta
    CartaGrafica(const std::string& rutaImagen, float x, float y);

    // Desenha a carta na janela
    void draw(sf::RenderWindow& window);

    // Obtém as dimensões/área ocupada pela carta
    sf::FloatRect getBounds() const;

    // Define a posição da carta na tela
    void setPosition(float x, float y);
};

#endif
