#include <SFML/Graphics.hpp>
#include <cmath>
#include "Button.cpp"
#include <chrono>

const std::uint16_t WINDOW_WIDTH = 1600;
const std::uint16_t WINDOW_HEIGHT = 900;
const float UPDATE_MS = 33;
const float MAXBOUNCEANGLE = (5 * 3.14159265) / 12;

enum GAME_STATE {
    MENU,
    IN_GAME,
    EXIT,
};

#include "PongGame.cpp"

class PongMenu {

private:
    Button m_playButton;
    Button m_exitButton;
    bool m_shouldStart;
    bool m_shouldExit;
    sf::RenderTarget& m_target;
    sf::Font& m_font;

public:

    PongMenu(sf::RenderTarget& target, sf::Font& font) :
        m_target(target),
        m_font(font),
        m_playButton("PLAY", { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 150, 65 }),
        m_exitButton("EXIT", { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 100, 140, 65 }),
        m_shouldStart(false),
        m_shouldExit(false)
    {
        m_playButton.setCallback([this]() {m_shouldStart = true; });
        m_exitButton.setCallback([this]() {m_shouldExit = true; });
    }
    /*
    Function to update the pong menu based off elapsed time
    */
    GAME_STATE Update(const float elapsedTime, Vector2D mousePos) {
        MOUSE_STATE mouse_state = MOUSE_STATE::M_UP;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) ||
            sf::Mouse::isButtonPressed(sf::Mouse::Middle) ||
            sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            mouse_state = MOUSE_STATE::M_DOWN;
        }

        m_playButton.handleInput(mousePos, mouse_state);
        m_exitButton.handleInput(mousePos, mouse_state);

        if (m_shouldStart) {
            return GAME_STATE::IN_GAME;
        }
        else if (m_shouldExit) {
            return GAME_STATE::EXIT;
        }
        return GAME_STATE::MENU;
    }

    /*
    Function to populate the screen with renderings based on elapsed time
    */
    void Render(const float elapsedTime) {
        m_playButton.Render(m_target, m_font);
        m_exitButton.Render(m_target, m_font);
    }

    void Reset() {
        m_shouldExit = false;
        m_shouldStart = false;
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake");
    sf::Font font;
    font.loadFromFile("Quicksand-Bold.otf");
    
    GAME_STATE gameState = MENU;
    PongMenu menu(window, font);
    PongGame game(window);

    std::chrono::system_clock::time_point lastTime = std::chrono::system_clock::now();
    float lag = 0;

    while (window.isOpen() && gameState != GAME_STATE::EXIT) {
        // check window events that were trigged since last iteration
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        std::chrono::system_clock::time_point currTime = std::chrono::system_clock::now();
        std::chrono::milliseconds elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currTime - lastTime);
        lastTime = currTime;
        lag += elapsedTime.count();

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (gameState == GAME_STATE::MENU) {
            gameState = menu.Update((UPDATE_MS), { static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) });
        }
        else {
            gameState = game.Update(static_cast<float>(UPDATE_MS), { static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) });
            if (gameState == GAME_STATE::MENU) {
                menu.Reset();
            }
        }
        
        window.clear();
        
        if (gameState == GAME_STATE::MENU) {
            menu.Render(static_cast<float>(elapsedTime.count()));
        }
        else if (gameState == GAME_STATE::IN_GAME) {
            game.Render();
        }

        window.display();
    }

    window.close();

    return 0;
}