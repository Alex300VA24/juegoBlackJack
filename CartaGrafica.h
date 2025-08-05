#ifndef CARTAGRAFICA_H
#define CARTAGRAFICA_H

#include <SFML/Graphics.hpp>
#include <string>

class CartaGrafica {
private:
    sf::Texture textura;
    sf::Sprite sprite;
    std::string nombreArchivo;

public:
    CartaGrafica(const std::string& rutaImagen, float x, float y);

    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    void setPosition(float x, float y);
};

#endif
