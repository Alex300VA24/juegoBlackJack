#include "CartaGrafica.h"
#include <iostream>

CartaGrafica::CartaGrafica(const std::string& rutaImagen, float x, float y) {
    if (!textura.loadFromFile(rutaImagen)) {
        std::cerr << "No se pudo cargar: " << rutaImagen << std::endl;
    }
    sprite.setTexture(textura);
    sprite.setPosition(x, y);
}

void CartaGrafica::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect CartaGrafica::getBounds() const {
    return sprite.getGlobalBounds();
}

void CartaGrafica::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}
