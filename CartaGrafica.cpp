#include "CartaGrafica.h"
#include <iostream>

// Construtor: tenta carregar a textura da carta a partir do arquivo
CartaGrafica::CartaGrafica(const std::string& rutaImagen, float x, float y) {
    if (!textura.loadFromFile(rutaImagen)) {
        std::cerr << "Não foi possível carregar: " << rutaImagen << std::endl;
    }
    sprite.setTexture(textura);
    sprite.setPosition(x, y);
}

// Desenha a carta na janela
void CartaGrafica::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Retorna os limites do sprite (para detecção de clique, sobreposição, etc.)
sf::FloatRect CartaGrafica::getBounds() const {
    return sprite.getGlobalBounds();
}

// Atualiza a posição do sprite na tela
void CartaGrafica::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}
