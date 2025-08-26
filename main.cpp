// BlackJack visual com SFML e lógica básica de jogador e crupiê
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "Jogo.h"

using namespace std;

Jogo jogo;

// Estrutura para representar uma carta na parte gráfica
struct CartaVisual {
    sf::Sprite sprite;               // Imagem da carta
    std::string nombreArquivo;       // Nome do arquivo da imagem
    sf::Vector2f destino;            // Posição final da animação
    bool animando = false;           // Flag para saber se está animando
};

// Vetores para armazenar cartas do jogador e do crupiê
std::vector<CartaVisual> cartasJogador;
std::vector<CartaVisual> cartasCrupier;
std::vector<Carta> cartasLogicasCrupier; // Cartas do crupiê na lógica (não só visuais)

// Variáveis para o resultado final da partida
std::string resultadoFinal = "";
sf::Text textoResultado;

Carta cartaOcultaCrupier;     // Primeira carta do crupiê (oculta até certo momento)
bool cartaRevelada = false;   // Flag para saber se a carta já foi revelada
bool turnoCrupier = false;    // Indica se é o turno do crupiê
bool crupierTerminado = false;// Indica se o crupiê terminou sua jogada
bool resultadoProcesado = false; // Para evitar processar o resultado mais de uma vez
sf::Clock relojCrupier;       // Relógio para controlar tempo entre cartas
float tiempoEntreCartas = 1.0f; // Tempo de atraso entre cartas do crupiê

// Recursos gráficos extras
std::vector<sf::Texture> texturasExtras;
std::map<std::string, sf::Texture> texturaMapa;

// Controle de dinheiro e apostas
int cash = 1000;       // Dinheiro inicial do jogador
int partida = 1;       // Número da partida
int apuesta = 0;       // Valor da aposta
bool juegoIniciado = false;  // Flag para indicar se a partida começou

// Valores possíveis para as fichas de aposta
std::vector<int> valoresFichas = {5, 10, 20, 50, 100, 200};
std::vector<sf::CircleShape> fichas;   // Representação visual das fichas
std::vector<sf::Text> textosFichas;    // Texto dentro das fichas



// ============================ FUNÇÕES ============================ //

// Calcula a pontuação de um conjunto de cartas
int calcularPontos(const std::vector<Carta>& cartas) {
    int total = 0, ases = 0;
    for (const auto& carta : cartas) {
        total += carta.peso;              // Soma o peso da carta
        if (carta.valor == "A") ases++;   // Conta quantos ases existem
    }
    // Se o total passar de 21 e houver ases, converte alguns de 11 para 1
    while (total > 21 && ases > 0) {
        total -= 10;
        ases--;
    }
    return total;
}

// Calcula a pontuação do crupiê, considerando se a carta oculta será incluída
int calcularPontosCrupierVisibles(const std::vector<Carta>& cartas, bool incluirOculta) {
    if (incluirOculta)
        return calcularPontos(cartas);

    // Se a carta ainda não foi revelada, ignora a primeira
    if (cartas.size() <= 1) return 0;

    std::vector<Carta> visibles(cartas.begin() + 1, cartas.end());
    return calcularPontos(visibles);
}


// Adiciona uma carta animada (com imagem e movimento até a posição final)
void adicionarCartaAnimada(std::vector<CartaVisual>& destino, std::map<std::string, sf::Texture>& mapa, const std::string& ruta, float finalX, float finalY) {
    // Carrega a textura da carta caso ainda não esteja no mapa
    if (mapa.find(ruta) == mapa.end()) {
        sf::Texture tex;
        if (!tex.loadFromFile(ruta)) {
            std::cerr << "Erro ao carregar carta: " << ruta << "\n";
            return;
        }
        mapa[ruta] = std::move(tex);
    }

    // Cria o sprite da carta
    sf::Sprite sprite;
    sprite.setTexture(mapa[ruta]);
    sprite.setScale(0.13f, 0.13f);       // Reduz a escala da carta
    sprite.setPosition(375, -130);       // Posição inicial (fora da tela)
    
    // Adiciona ao vetor de cartas visuais
    destino.push_back({ sprite, ruta, sf::Vector2f(finalX, finalY), true });
}

// Retorna o nome da imagem correspondente a uma carta
std::string nomeImagemCarta(const Carta& carta) {
    return "assets/cartao/" + carta.valor + "_of_" + carta.naipe + ".png";
}



int main() {
    // Criação da janela principal do jogo (750x422, título "BlackJack")
    sf::RenderWindow window(sf::VideoMode(750, 422), "BlackJack", sf::Style::Close);
    window.setFramerateLimit(100); // limita FPS para 100, evitando sobrecarga

    // Fundo e título
    sf::Texture backgroundTex;
    backgroundTex.loadFromFile("assets/fond1.png"); // carrega imagem de fundo
    sf::Sprite background(backgroundTex);

    sf::Font font;
    font.loadFromFile("assets/casino/Casino.ttf"); // fonte estilo "casino"

    sf::Text titulo("BLACK JACK", font, 40);
    titulo.setPosition(300, -5);
    titulo.setFillColor(sf::Color::White);

    sf::Text regla("Dealer must draw to 16 & stand on 17", font, 16);
    regla.setPosition(260, 45);
    regla.setFillColor(sf::Color::Cyan);

    // Informações do jogador
    sf::Text textoCash("DINHEIRO: 1000 reais", font, 18);
    textoCash.setPosition(20, 20);
    textoCash.setFillColor(sf::Color::White);

    sf::Text textoApuesta("APOSTA: 0 reais", font, 18);
    textoApuesta.setPosition(20, 50);
    textoApuesta.setFillColor(sf::Color::White);


    // Fichas de apostas
    for (size_t i = 0; i < valoresFichas.size(); ++i) {
        sf::CircleShape ficha(25); // círculo que representa uma ficha
        ficha.setFillColor(sf::Color(100 + i * 20, 100, 255));
        ficha.setPosition(150 + i * 80, 250); // posicionadas em linha
        fichas.push_back(ficha);

        sf::Text t;
        t.setFont(font);
        t.setCharacterSize(16);
        t.setFillColor(sf::Color::White);
        t.setString(std::to_string(valoresFichas[i])); // valor da ficha escrito
        t.setPosition(ficha.getPosition().x + 15, ficha.getPosition().y + 15);
        textosFichas.push_back(t);
    }

    // Botão "APOSTAR"
    sf::RectangleShape btnApostar(sf::Vector2f(120, 40));
    btnApostar.setFillColor(sf::Color::Green);
    btnApostar.setPosition(330, 350);

    sf::Text txtApostar("APOSTAR", font, 20);
    txtApostar.setFillColor(sf::Color::Black);
    txtApostar.setPosition(345, 355);


    // Mensagens de resultado
    textoResultado.setFont(font);
    textoResultado.setCharacterSize(32);
    textoResultado.setFillColor(sf::Color::White);
    textoResultado.setStyle(sf::Text::Bold);

    sf::Text txtLost("Voce Perdeu!!", font, 32);
    txtLost.setFillColor(sf::Color::Red);
    txtLost.setPosition(375 - txtLost.getLocalBounds().width / 2, 150);

    // Carrega imagens dos botões
    sf::Texture texHit, texStand;
    if (!texHit.loadFromFile("assets/boton_hit.png")) std::cerr << "No se cargó hit.png\n";
    if (!texStand.loadFromFile("assets/boton_stand.png")) std::cerr << "No se cargó stand.png\n";

    // Botão "HIT" (pedir carta)
    sf::Sprite btnHit(texHit);
    btnHit.setPosition(515, 300);
    btnHit.setScale(0.5f, 0.5f); // ajusta tamanho
    sf::Text txtHit("HIT", font, 20);
    txtHit.setFillColor(sf::Color::Yellow);
    txtHit.setPosition(545, 385);

    // Botão "STAND" (parar)
    sf::Sprite btnStand(texStand);
    btnStand.setPosition(600, 300);
    btnStand.setScale(0.5f, 0.5f);
    sf::Text txtStand("STAND", font, 20);
    txtStand.setFillColor(sf::Color::Yellow);
    txtStand.setPosition(620, 385);

    // Botão "RETORNAR"
    sf::RectangleShape btnReturn(sf::Vector2f(120, 40));
    btnReturn.setPosition(600, 200);
    btnReturn.setFillColor(sf::Color::Green);

    sf::Text txtReturn("RETORNAR", font, 18);
    txtReturn.setFillColor(sf::Color::Black);
    txtReturn.setPosition(630, 205);

    // Pontuação atual
    sf::Text puntosJugador("JOGADOR: 0", font, 20);
    puntosJugador.setPosition(20, 370);
    puntosJugador.setFillColor(sf::Color::White);

    sf::Text puntosCrupier("CRUPIER: 0", font, 20);
    puntosCrupier.setPosition(20, 90);
    puntosCrupier.setFillColor(sf::Color::White);

    // Inicializar crupier: carta oculta e visível
    Carta cartaVisibleCrupier;

    bool cartaAgregada = false; // controla se já foi adicionada carta inicial
    bool juegoCorriendo = true; // flag para manter o loop principal rodando


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
        // Loop principal de eventos da janela
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // Fecha a janela se o usuário clicar no "X"
            
            // Detecta clique do mouse
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mouseF(mousePos.x, mousePos.y);
                
                // Se o jogo já tiver terminado (resultadoFinal não vazio), não processa ações
                if (resultadoFinal.empty()) {
                    
                    // --- Botão "Hit" (pedir carta) ---
                    if (btnHit.getGlobalBounds().contains(mouseF)) {
                        // Pega uma nova carta do baralho
                        Carta novaCarta = jogo.tirarCarta();
                        std::string arquivo = nomeImagemCarta(novaCarta);

                        // Adiciona a carta na mão do jogador com animação
                        adicionarCartaAnimada(cartasJogador, texturaMapa, arquivo, 250 + cartasJogador.size() * 50, 300);

                        // Recalcula os pontos do jogador com base nas cartas atuais
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
                        
                        // Verifica se o jogador estourou (> 21 pontos)
                        int puntosJugador = calcularPontos(logicasJugadorTemp);
                        if (puntosJugador > 21) {
                            resultadoFinal = "Crupier Wins!";
                            cartaRevelada = true;
                            crupierTerminado = true;
                            turnoCrupier = false;

                            // Revela a carta oculta do crupiê
                            texturasExtras.emplace_back();
                            if (texturasExtras.back().loadFromFile(nomeImagemCarta(cartaOcultaCrupier))) {
                                cartasCrupier[0].sprite.setTexture(texturasExtras.back());
                            }

                            // Exibe o resultado na tela
                            textoResultado.setString(resultadoFinal);
                            textoResultado.setPosition(375 - textoResultado.getLocalBounds().width / 2, 200);
                        }
                    }

                    // --- Botão "Stand" (parar e passar vez) ---
                    if (btnStand.getGlobalBounds().contains(mouseF)) {
                        if (!turnoCrupier) {
                            turnoCrupier = true; // passa vez para o crupiê
                            relojCrupier.restart();

                            // Revela a carta oculta imediatamente
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
                // --- Se o jogo ainda não começou (fase de apostas) ---
                if (!juegoIniciado) {
                    for (size_t i = 0; i < fichas.size(); ++i) {
                        if (fichas[i].getGlobalBounds().contains(mouseF)) {
                            int valor = valoresFichas[i];
                            if (apuesta + valor <= cash) {
                                apuesta += valor; // adiciona valor à aposta
                            }
                        }
                    }

                    // Botão "Apostar" -> inicia o jogo
                    if (btnApostar.getGlobalBounds().contains(mouseF)) {
                        if (apuesta > 0 && apuesta <= cash) {
                            cash -= apuesta; // desconta aposta
                            juegoIniciado = true;
                            
                            // Distribui cartas iniciais
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
        
        // --- Lógica de animação das cartas (movimento até o destino) ---
        float velocidade = 20.0f;

        /* movimenta cartas do jogador */
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
        /* movimenta cartas do crupiê */
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
        // --- Turno do crupiê (compra cartas até >= 17 pontos) ---
        if (turnoCrupier && !crupierTerminado && relojCrupier.getElapsedTime().asSeconds() > tiempoEntreCartas) {
            int puntos = calcularPontos(cartasLogicasCrupier);
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
        // --- Cálculo dos pontos em tempo real ---
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

        // --- Quando o crupiê termina, processa resultado final ---
        if (crupierTerminado && !resultadoProcesado) {
            /* verifica quem venceu: jogador, crupiê, empate, blackjack etc. */
            texturasExtras.emplace_back();
            resultadoProcesado = true;
            if (texturasExtras.back().loadFromFile(nomeImagemCarta(cartaOcultaCrupier))) {
                cartasCrupier[0].sprite.setTexture(texturasExtras.back());
            }
            // Mostrar resultado
            int puntosJugador = calcularPontos(logicasJugador);
            int puntosCrupier = calcularPontos(cartasLogicasCrupier);

            bool blackjackJugador = (logicasJugador.size() == 2 && calcularPontos(logicasJugador) == 21);
            bool blackjackCrupier = (cartasLogicasCrupier.size() == 2 && calcularPontos(cartasLogicasCrupier) == 21);

            if (blackjackJugador && !blackjackCrupier) {
                resultadoFinal = "Blackjack! You Win!";
                cash += apuesta + int(apuesta * 1.5); // ganancia de 3:2
            } else if (!blackjackJugador && blackjackCrupier) {
                resultadoFinal = "Crupier Blackjack. You Lose!";
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
        // --- Atualiza textos de dinheiro, aposta e pontos ---
        textoCash.setString("DINHEIRO: " + std::to_string(cash) + " reais");
        textoApuesta.setString("APOSTA: " + std::to_string(apuesta) + " reais");


        totalJugador = calcularPontos(logicasJugador);
        totalCrupier = calcularPontosCrupierVisibles(cartasLogicasCrupier, cartaRevelada);

        // --- Renderização dos elementos na tela ---
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
