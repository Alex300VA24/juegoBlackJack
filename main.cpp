// BlackJack visual con SFML y lógica básica de jugador y crupier
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "Jogo.h"

Jogo jogo;

struct CartaVisual {
    sf::Sprite sprite;
    std::string nombreArquivo;
    sf::Vector2f destino;
    bool animando = false;
};

std::vector<CartaVisual> cartasJogador;
std::vector<CartaVisual> cartasCrupier;
std::vector<Carta> cartasLogicasCrupier;
std::string resultadoFinal = "";
sf::Text textoResultado;

Carta cartaOcultaCrupier;
bool cartaRevelada = false;
bool turnoCrupier = false;
bool crupierTerminado = false;
bool resultadoProcesado = false;
sf::Clock relojCrupier;
float tiempoEntreCartas = 1.0f;
std::vector<sf::Texture> texturasExtras;
std::map<std::string, sf::Texture> texturaMapa;
int cash = 1000;
int partida = 1;
int apuesta = 0;
bool juegoIniciado = false;
std::vector<int> valoresFichas = {5, 10, 20, 50, 100, 200};
std::vector<sf::CircleShape> fichas;
std::vector<sf::Text> textosFichas;



// Calcular puntos
int calcularPuntos(const std::vector<Carta>& cartas) {
    int total = 0, ases = 0;
    for (const auto& carta : cartas) {
        total += carta.peso;
        if (carta.valor == "A") ases++;
    }
    while (total > 21 && ases > 0) {
        total -= 10;
        ases--;
    }
    return total;
}

// Calcular puntos del crupier con cartas visibles
int calcularPuntosCrupierVisibles(const std::vector<Carta>& cartas, bool incluirOculta) {
    if (incluirOculta)
        return calcularPuntos(cartas);

    // Si no se revela aún, ignorar la primera carta
    if (cartas.size() <= 1) return 0;

    std::vector<Carta> visibles(cartas.begin() + 1, cartas.end());
    return calcularPuntos(visibles);
}


// Agregar carta con su imagen
void adicionarCartaAnimada(std::vector<CartaVisual>& destino, std::map<std::string, sf::Texture>& mapa, const std::string& ruta, float finalX, float finalY) {
    if (mapa.find(ruta) == mapa.end()) {
        sf::Texture tex;
        if (!tex.loadFromFile(ruta)) {
            std::cerr << "Erro ao carregar carta: " << ruta << "\n";
            return;
        }
        mapa[ruta] = std::move(tex);
    }

    sf::Sprite sprite;
    sprite.setTexture(mapa[ruta]);
    sprite.setScale(0.13f, 0.13f);
    sprite.setPosition(375, -130);
    destino.push_back({ sprite, ruta, sf::Vector2f(finalX, finalY), true });
}

// Obtener carta para renderizar
std::string nomeImagemCarta(const Carta& carta) {
    return "assets/cartao/" + carta.valor + "_of_" + carta.naipe + ".png";
}




int main() {
    sf::RenderWindow window(sf::VideoMode(750, 422), "BlackJack", sf::Style::Close);
    window.setFramerateLimit(100);

    sf::Texture backgroundTex;
    backgroundTex.loadFromFile("assets/fond1.png");
    sf::Sprite background(backgroundTex);

    sf::Font font;
    font.loadFromFile("assets/casino/Casino.ttf");

    sf::Text titulo("BLACK JACK", font, 40);
    titulo.setPosition(300, -5);
    titulo.setFillColor(sf::Color::White);

    sf::Text regla("Dealer must draw to 16 & stand on 17", font, 16);
    regla.setPosition(260, 45);
    regla.setFillColor(sf::Color::Cyan);

    sf::Text textoCash("DINHEIRO: 1000 reais", font, 18);
    textoCash.setPosition(20, 20);
    textoCash.setFillColor(sf::Color::White);

    sf::Text textoApuesta("APOSTA: 0 reais", font, 18);
    textoApuesta.setPosition(20, 50);
    textoApuesta.setFillColor(sf::Color::White);

    // Fichas

    for (size_t i = 0; i < valoresFichas.size(); ++i) {
        sf::CircleShape ficha(25);
        ficha.setFillColor(sf::Color(100 + i * 20, 100, 255));
        ficha.setPosition(150 + i * 80, 250); // distribuidas horizontalmente
        fichas.push_back(ficha);

        sf::Text t;
        t.setFont(font);
        t.setCharacterSize(16);
        t.setFillColor(sf::Color::White);
        t.setString(std::to_string(valoresFichas[i]));
        t.setPosition(ficha.getPosition().x + 12, ficha.getPosition().y + 8);
        textosFichas.push_back(t);
    }

    sf::RectangleShape btnApostar(sf::Vector2f(120, 40));
    btnApostar.setFillColor(sf::Color::Green);
    btnApostar.setPosition(330, 350);

    sf::Text txtApostar("APOSTAR", font, 20);
    txtApostar.setFillColor(sf::Color::Black);
    txtApostar.setPosition(345, 355);




    // Texto de resultados
    textoResultado.setFont(font);
    textoResultado.setCharacterSize(32);
    textoResultado.setFillColor(sf::Color::White);
    textoResultado.setStyle(sf::Text::Bold);

    sf::Text txtLost("Voce Perdeu!!", font, 32);
    txtLost.setFillColor(sf::Color::Red);
    txtLost.setPosition(375 - txtLost.getLocalBounds().width / 2, 150);

    // Cargar botones como imágenes
    sf::Texture texHit, texStand;
    if (!texHit.loadFromFile("assets/boton_hit.png")) std::cerr << "No se cargó hit.png\n";
    if (!texStand.loadFromFile("assets/boton_stand.png")) std::cerr << "No se cargó stand.png\n";

    sf::Sprite btnHit(texHit);
    btnHit.setPosition(515, 300);
    btnHit.setScale(0.5f, 0.5f); // escala opcional según tamaño

    sf::Text txtHit("HIT", font, 20);
    txtHit.setFillColor(sf::Color::Yellow);
    txtHit.setPosition(545, 385);

    sf::Sprite btnStand(texStand);
    btnStand.setPosition(600, 300);
    btnStand.setScale(0.5f, 0.5f);

    sf::Text txtStand("STAND", font, 20);
    txtStand.setFillColor(sf::Color::Yellow);
    txtStand.setPosition(620, 385);

    // Botao return
    sf::RectangleShape btnReturn(sf::Vector2f(120, 40));
    btnReturn.setPosition(600, 200);
    btnReturn.setFillColor(sf::Color::Green);

    sf::Text txtReturn("RETORNAR", font, 18);
    txtReturn.setFillColor(sf::Color::Black);
    txtReturn.setPosition(630, 205);


    sf::Text puntosJugador("JOGADOR: 0", font, 20);
    puntosJugador.setPosition(20, 370);
    puntosJugador.setFillColor(sf::Color::White);

    sf::Text puntosCrupier("CRUPIER: 0", font, 20);
    puntosCrupier.setPosition(20, 90);
    puntosCrupier.setFillColor(sf::Color::White);


    // Inicializar crupier: carta oculta y visible
    //cartaOcultaCrupier = jogo.tirarCarta();
    Carta cartaVisibleCrupier;
    //cartasLogicasCrupier.push_back(cartaOcultaCrupier);
    //cartasLogicasCrupier.push_back(cartaVisibleCrupier);
    //adicionarCartaAnimada(cartasCrupier, texturaMapa, "assets/reverso.png", 250, 80);
    //adicionarCartaAnimada(cartasCrupier, texturaMapa, nomeImagemCarta(cartaVisibleCrupier), 300, 80);

    bool cartaAgregada = false;
    bool juegoCorriendo = true;

    while (window.isOpen() && juegoCorriendo) {


        sf::Event event;

        if (!resultadoFinal.empty() && event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mouseF(mousePos.x, mousePos.y);

            if (btnReturn.getGlobalBounds().contains(mouseF)) {
                // Reiniciar juego
                cartasJogador.clear();
                cartasCrupier.clear();
                cartasLogicasCrupier.clear();
                resultadoFinal = "";
                cartaRevelada = false;
                turnoCrupier = false;
                crupierTerminado = false;
                juegoIniciado = false;
                apuesta = 0;
                cartaVisibleCrupier = Carta();
                cartaOcultaCrupier = Carta();

                // Actualizar textos
                puntosJugador.setString("JOGADOR: 0");
                puntosCrupier.setString("CRUPIER: 0");

                texturasExtras.clear(); // limpiar si usaste texturas dinámicas

                // Opcional: volver a posición inicial si usas animaciones
                for (auto& carta : cartasJogador)
                    carta.animando = false;
                for (auto& carta : cartasCrupier)
                    carta.animando = false;

                resultadoProcesado = false;
            }
        }

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mouseF(mousePos.x, mousePos.y);

                if (resultadoFinal.empty()) {

                    if (btnHit.getGlobalBounds().contains(mouseF)) {
                        // HIT
                        Carta novaCarta = jogo.tirarCarta();
                        std::string arquivo = nomeImagemCarta(novaCarta);
                        adicionarCartaAnimada(cartasJogador, texturaMapa, arquivo, 250 + cartasJogador.size() * 50, 300);

                        // Calcular puntos actualizados
                        std::vector<Carta> logicasJugadorTemp;

                        for (const auto& visual : cartasJogador) {
                            std::string nombre = visual.nombreArquivo;
                            size_t barra = nombre.find_last_of('/');
                            size_t under = nombre.find("_of_");
                            size_t punto = nombre.find(".png");

                            if (under != std::string::npos && punto != std::string::npos) {
                                std::string valor = nombre.substr(barra + 1, under - barra - 1);
                                std::string naipe = nombre.substr(under + 4, punto - under - 4);
                                int peso = 0;
                                if (valor == "A") peso = 11;
                                else if (valor == "J" || valor == "Q" || valor == "K") peso = 10;
                                else peso = std::stoi(valor);

                                logicasJugadorTemp.push_back({valor, naipe, peso});
                            }
                        }

                        int puntosJugador = calcularPuntos(logicasJugadorTemp);
                        if (puntosJugador > 21) {
                            resultadoFinal = "Crupier Wins!";
                            cartaRevelada = true;
                            crupierTerminado = true;
                            turnoCrupier = false;

                            // Mostrar carta oculta del crupier
                            texturasExtras.emplace_back();
                            if (texturasExtras.back().loadFromFile(nomeImagemCarta(cartaOcultaCrupier))) {
                                cartasCrupier[0].sprite.setTexture(texturasExtras.back());
                            }

                            textoResultado.setString(resultadoFinal);
                            textoResultado.setPosition(375 - textoResultado.getLocalBounds().width / 2, 200);
                        }
                    }


                    if (btnStand.getGlobalBounds().contains(mouseF)) {
                        if (!turnoCrupier) {
                            turnoCrupier = true;
                            relojCrupier.restart();

                            // Revelar la carta oculta del crupier inmediatamente
                            if (!cartaRevelada) {
                                texturasExtras.emplace_back();
                                if (texturasExtras.back().loadFromFile(nomeImagemCarta(cartaOcultaCrupier))) {
                                    cartasCrupier[0].sprite.setTexture(texturasExtras.back());
                                    cartaRevelada = true;
                                }
                            }
                        }
                    }

                }
                if (!juegoIniciado) {
                    for (size_t i = 0; i < fichas.size(); ++i) {
                        if (fichas[i].getGlobalBounds().contains(mouseF)) {
                            int valor = valoresFichas[i];
                            if (apuesta + valor <= cash) {
                                apuesta += valor;
                            }
                        }
                    }

                    if (btnApostar.getGlobalBounds().contains(mouseF)) {
                        if (apuesta > 0 && apuesta <= cash) {
                            cash -= apuesta;
                            juegoIniciado = true;
                            // aquí comienzas a repartir cartas al jugador y crupier

                            cartaOcultaCrupier = jogo.tirarCarta();
                            cartasLogicasCrupier.push_back(cartaOcultaCrupier);
                            adicionarCartaAnimada(cartasCrupier, texturaMapa, "assets/reverso.png", 250, 80);

                            cartaVisibleCrupier = jogo.tirarCarta();
                            cartasLogicasCrupier.push_back(cartaVisibleCrupier);
                            adicionarCartaAnimada(cartasCrupier, texturaMapa, nomeImagemCarta(cartaVisibleCrupier), 300, 80);

                            Carta cartaJogador = jogo.tirarCarta();
                            std::string arquivo = nomeImagemCarta(cartaJogador);
                            adicionarCartaAnimada(cartasJogador, texturaMapa, arquivo, 250 + cartasJogador.size() * 50, 300);

                            

                        }
                    }
                }

            }

        }

        float velocidade = 20.0f;
        for (auto& carta : cartasJogador) {
            if (carta.animando) {
                sf::Vector2f pos = carta.sprite.getPosition();
                sf::Vector2f dir = carta.destino - pos;
                float distancia = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                if (distancia <= velocidade) {
                    carta.sprite.setPosition(carta.destino);
                    carta.animando = false;
                } else {
                    sf::Vector2f paso = (dir / distancia) * velocidade;
                    carta.sprite.move(paso);
                }
            }
        }
        for (auto& carta : cartasCrupier) {
            if (carta.animando) {
                sf::Vector2f pos = carta.sprite.getPosition();
                sf::Vector2f dir = carta.destino - pos;
                float distancia = std::sqrt(dir.x * dir.x + dir.y * dir.y);

                if (distancia <= velocidade) {
                    carta.sprite.setPosition(carta.destino);
                    carta.animando = false;
                } else {
                    sf::Vector2f paso = (dir / distancia) * velocidade;
                    carta.sprite.move(paso);
                }
            }
        }
        if (turnoCrupier && !crupierTerminado && relojCrupier.getElapsedTime().asSeconds() > tiempoEntreCartas) {
            int puntos = calcularPuntos(cartasLogicasCrupier);
            if (puntos < 17) {
                Carta novaCarta = jogo.tirarCarta();
                std::string img = nomeImagemCarta(novaCarta);
                float posX = 250 + cartasCrupier.size() * 50;
                adicionarCartaAnimada(cartasCrupier, texturaMapa, img, posX, 80);
                cartasLogicasCrupier.push_back(novaCarta);
                relojCrupier.restart();
            } else {
                crupierTerminado = true;
            }
        }
        // --- calcular puntos en tiempo real ---
        int totalJugador = 0;
        int totalCrupier = 0;
        std::vector<Carta> logicasJugador;  // cartas que se han repartido

        // simular cartas del jugador basadas en imagen
        for (const auto& visual : cartasJogador) {
            // recuperar el nombre de archivo como "10_of_hearts.png"
            std::string nombre = visual.nombreArquivo;
            size_t barra = nombre.find_last_of('/');
            size_t under = nombre.find("_of_");
            size_t punto = nombre.find(".png");

            if (under != std::string::npos && punto != std::string::npos) {
                std::string valor = nombre.substr(barra + 1, under - barra - 1);
                std::string naipe = nombre.substr(under + 4, punto - under - 4);
                int peso = 0;
                if (valor == "A") peso = 11;
                else if (valor == "J" || valor == "Q" || valor == "K") peso = 10;
                else peso = std::stoi(valor);

                logicasJugador.push_back({valor, naipe, peso});
            }
        }

        if (crupierTerminado && !resultadoProcesado) {
            texturasExtras.emplace_back();
            resultadoProcesado = true;
            if (texturasExtras.back().loadFromFile(nomeImagemCarta(cartaOcultaCrupier))) {
                cartasCrupier[0].sprite.setTexture(texturasExtras.back());
            }
            // Mostrar resultado
            int puntosJugador = calcularPuntos(logicasJugador);
            int puntosCrupier = calcularPuntos(cartasLogicasCrupier);

            bool blackjackJugador = (logicasJugador.size() == 2 && calcularPuntos(logicasJugador) == 21);
            bool blackjackCrupier = (cartasLogicasCrupier.size() == 2 && calcularPuntos(cartasLogicasCrupier) == 21);

            if (blackjackJugador && !blackjackCrupier) {
                resultadoFinal = "Blackjack! You Win!";
                cash += apuesta + int(apuesta * 1.5); // ganancia de 3:2
            } else if (!blackjackJugador && blackjackCrupier) {
                resultadoFinal = "Crupier has Blackjack. You Lose!";
                // no suma nada
            } else if (blackjackJugador && blackjackCrupier) {
                resultadoFinal = "Double Blackjack. Draw!";
                cash += apuesta; // devuelve apuesta
            } else if (puntosJugador > 21) {
                resultadoFinal = "Crupier Wins!";
            } else if (puntosCrupier > 21) {
                resultadoFinal = "You Win!";
                cash += apuesta * 2;
            } else if (puntosJugador > puntosCrupier) {
                resultadoFinal = "You Win!";
                cash += apuesta * 2;
            } else if (puntosJugador < puntosCrupier) {
                resultadoFinal = "Crupier Wins!";
                // no suma
            } else {
                resultadoFinal = "Draw!";
                cash += apuesta; // apuesta devuelta
            }

            textoResultado.setString(resultadoFinal);
            textoResultado.setPosition(375 - textoResultado.getLocalBounds().width / 2, 200);

        }
        textoCash.setString("DINHEIRO: " + std::to_string(cash) + " reais");
        textoApuesta.setString("APOSTA: " + std::to_string(apuesta) + " reais");


        totalJugador = calcularPuntos(logicasJugador);
        totalCrupier = calcularPuntosCrupierVisibles(cartasLogicasCrupier, cartaRevelada);


        window.clear();
        window.draw(background);
        window.draw(titulo);
        window.draw(regla);
        window.draw(textoCash);
        window.draw(textoApuesta);
        window.draw(puntosJugador);
        window.draw(puntosCrupier);

        if (!resultadoFinal.empty()) {
            window.draw(textoResultado);
        }
        if (!resultadoFinal.empty()) {
            window.draw(btnReturn);
            window.draw(txtReturn);
        }
        if (!juegoIniciado) {
            for (const auto& ficha : fichas) {
                window.draw(ficha);
            }
            for (const auto& texto : textosFichas) {
                window.draw(texto);
            }
        }

        if (!juegoIniciado) {
            for (auto& ficha : fichas) window.draw(ficha);
            for (auto& t : textosFichas) window.draw(t);
            window.draw(btnApostar);
            window.draw(txtApostar);
            if (cash == 0){
                    window.draw(txtLost);
            }
        }
        if (juegoIniciado){
            for (auto& carta : cartasJogador)
                window.draw(carta.sprite);
            for (auto& carta : cartasCrupier)
                window.draw(carta.sprite);
            
            window.draw(btnHit);
            window.draw(txtHit);
            window.draw(btnStand);
            window.draw(txtStand);
            // Actualizar textos
            puntosJugador.setString("JOGADOR: " + std::to_string(totalJugador));
            puntosCrupier.setString("CRUPIER: " + std::to_string(totalCrupier));

        }


        window.display();
    }
    return 0;
}
