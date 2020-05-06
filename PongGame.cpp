enum SERVE_STATE {
    PLAYER_ONE,
    PLAYER_TWO, 
    MIDDLE
};

enum INGAME_STATE {
    PLAYING,
    PAUSED
};

enum BALL_STATE {
    UP,
    HOVER,
    CLICKED
};

typedef std::function<void(void)> CallbackFunc;

class Court {
private:
    RectangleShape dimensions;
public:
    Court(RectangleShape dimensions) {
        this->dimensions = dimensions;
    }

    RectangleShape getDimensions() {
        return this->dimensions;
    }

    void Render(sf::RenderTarget& target) {
        sf::RectangleShape bg;
        sf::RectangleShape main;

        bg.setPosition({ dimensions.x, dimensions.y });
        bg.setSize({ dimensions.width, dimensions.height });
        bg.setFillColor(sf::Color::Red);

        main.setPosition({ 0, dimensions.y+4 });
        main.setSize({ WINDOW_WIDTH, WINDOW_HEIGHT - 8 });
        main.setFillColor(sf::Color::Black);

        target.draw(bg);
        target.draw(main);
    }

};

class Paddle {
private:
    RectangleShape m_paddle;
    Vector2D m_velocity;

public:
    Paddle(Vector2D starting_pos) {
        m_paddle.x = starting_pos.x;
        m_paddle.y = starting_pos.y;
        m_paddle.width = 10;
        m_paddle.height = 80;
        m_velocity = { 0, 2 };
    }

    void setVelocity(Vector2D velocity) {
        this->m_velocity = velocity;
    }

    Vector2D getVelocity() {
        return m_velocity;
    }

    void setPositionSize(RectangleShape pos_size) {
        this->m_paddle = pos_size;
    }

    RectangleShape getPositionSize() {
        return this->m_paddle;
    }

    void Render(sf::RenderTarget& target) {
        sf::RectangleShape paddle;
        paddle.setPosition({ m_paddle.x, m_paddle.y });
        paddle.setSize({ m_paddle.width, m_paddle.height });
        paddle.setFillColor(sf::Color::White);
        target.draw(paddle);
    }
};

class Ball {
private:
    float radius;
    Vector2D pos;
    Vector2D vel;
    sf::RenderTarget& target;
    CallbackFunc callback;
    BALL_STATE b_state;

public:
    Ball() :
        pos({ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 }),
        vel({ 2, 0 }),
        radius(5),
        target(target),
        callback([](){}),
        b_state(BALL_STATE::UP)
    {}

    void setPosition(Vector2D pos) {
        this->pos = pos;
    }

    Vector2D getPosition() {
        return this->pos;
    }

    void setVelocity(Vector2D vel) {
        this->vel = vel;
    }

    Vector2D getVelocity() {
        return this->vel;
    }

    void setRadius(float radius) {
        this->radius = radius;
    }

    void setCallback(CallbackFunc c) {
        this->callback = c;
    }

    // Function to start the game upon ball click
    bool handleInput(INGAME_STATE& game_state) {

        if (game_state == PLAYING) {
            callback();
            return true;
        }

        return false;
    }

    void Render(sf::RenderTarget& target) {
        sf::CircleShape ball(radius);
        ball.setPosition({ pos.x, pos.y });
        ball.setFillColor(sf::Color::White);
        target.draw(ball);
    }
};

class PongGame {
private:
    int playerOneScore;
    int playerTwoScore;
    int maxScore;
    Court court;
    Paddle paddle_one;
    Paddle paddle_two;
    Ball ball;
    INGAME_STATE ingame_state;
    sf::RenderTarget& m_target;
    bool ball_forward;
    bool ball_backward;

public:

    PongGame(sf::RenderTarget& target) :
        m_target(target),
        playerOneScore(0),
        playerTwoScore(0),
        maxScore(5),
        court({0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}),
        paddle_one({0, WINDOW_HEIGHT/2}),
        paddle_two({WINDOW_WIDTH - 10, WINDOW_HEIGHT/2}),
        ingame_state(PLAYING),
        ball_forward(true),
        ball_backward(false)
    {
        // ball.setCallback([this]() {ball_off_one = true; });
    }

    SERVE_STATE handleServeState(Vector2D ball_pos, Paddle paddle_one, Paddle paddle_two) {
        if (std::floor(ball_pos.x) <= 0 || (ingame_state == PAUSED && ball_pos.x == paddle_two.getPositionSize().x - paddle_two.getPositionSize().width)) {
            return SERVE_STATE::PLAYER_TWO;
        }
        else if (std::floor(ball_pos.x) >= WINDOW_WIDTH || (ingame_state == PAUSED && ball_pos.x == paddle_one.getPositionSize().x + paddle_one.getPositionSize().width)) {
            return SERVE_STATE::PLAYER_ONE;
        }
        return SERVE_STATE::MIDDLE;
    }

    void ballForward(Vector2D& ball_pos, float elapsedTime, float& speed, RectangleShape& paddle_pos2) {
        ball_pos = { ball_pos.x + ((ball.getVelocity().x * elapsedTime) / 1000), ball_pos.y - ((ball.getVelocity().y * elapsedTime) / 1000) };
        ball.setPosition(ball_pos);

        if (std::floor(ball_pos.x) == paddle_pos2.x && (std::floor(ball_pos.y) >= paddle_pos2.y &&
            std::floor(ball_pos.y) <= paddle_pos2.y + paddle_pos2.height)) {
            float rel_intersect_y1 = (paddle_pos2.y + paddle_pos2.height / 2) - ball_pos.y;
            float norm_intersect_y1 = rel_intersect_y1 / (paddle_pos2.height / 2);
            float bounceAngle1 = norm_intersect_y1 * MAXBOUNCEANGLE;
            ball.setVelocity({ speed * cos(bounceAngle1), speed * -sin(bounceAngle1) });
            ball_forward = false;
            ball_backward = true;
        }
    }

    void ballBackward(Vector2D& ball_pos, float elapsedTime, float& speed, RectangleShape& paddle_pos1) {
        ball_pos = { ball_pos.x - ((ball.getVelocity().x * elapsedTime) / 1000), ball_pos.y - ((ball.getVelocity().y * elapsedTime) / 1000) };
        ball.setPosition(ball_pos);

        if (std::floor(ball_pos.x) == paddle_pos1.x + paddle_pos1.width && (std::floor(ball_pos.y) >= paddle_pos1.y &&
            std::floor(ball_pos.y) <= paddle_pos1.y + paddle_pos1.height)) {
            float rel_intersect_y2 = (paddle_pos1.y + paddle_pos1.height / 2) - ball_pos.y;
            float norm_intersect_y2 = rel_intersect_y2 / (paddle_pos1.height / 2);
            float bounceAngle2 = norm_intersect_y2 * MAXBOUNCEANGLE;
            ball.setVelocity({ speed * cos(bounceAngle2), speed * -sin(bounceAngle2) });
            ball_forward = true;
            ball_backward = false;
        }
    }

    // Insert game logic here
    GAME_STATE Update(const float elapsedTime, Vector2D mouse_pos) {
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            ball.setPosition({ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 });
            return GAME_STATE::MENU;
        }

        /*
        -----------------------
        Move the paddles
        -----------------------
        */
        RectangleShape paddle_pos1 = paddle_one.getPositionSize();
        RectangleShape paddle_pos2 = paddle_two.getPositionSize();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && (std::floor(paddle_pos1.y) != 0)) {
            paddle_pos1.y -= paddle_one.getVelocity().y * elapsedTime / 1000;
            paddle_one.setPositionSize(paddle_pos1);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && (std::floor(paddle_pos1.y + paddle_pos1.height) != WINDOW_HEIGHT)) {
            paddle_pos1.y += paddle_one.getVelocity().y * elapsedTime / 1000;
            paddle_one.setPositionSize(paddle_pos1);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && (std::floor(paddle_pos2.y) != 0)) {
            paddle_pos2.y -= paddle_two.getVelocity().y * elapsedTime / 1000;
            paddle_two.setPositionSize(paddle_pos2);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && (std::floor(paddle_pos2.y + paddle_pos2.height) != WINDOW_HEIGHT)) {
            paddle_pos2.y += paddle_two.getVelocity().y * elapsedTime / 1000;
            paddle_two.setPositionSize(paddle_pos2);
        }
        /*
        -------------------
        End moving paddles
        -------------------
        */

        /*
        Logic to implement ball position on paddle collision
        */
        Vector2D ball_pos = ball.getPosition();
        float speed = sqrt(pow(ball.getVelocity().x, 2) + pow(ball.getVelocity().y, 2));
        SERVE_STATE serve_state = handleServeState(ball_pos, paddle_one, paddle_two);

        /*
        Restart game if the serve state is either player one or player two and one to score of respective player
        */
        if (serve_state == SERVE_STATE::PLAYER_ONE) {
            ball_pos = { paddle_one.getPositionSize().x + paddle_one.getPositionSize().width, paddle_one.getPositionSize().y + paddle_one.getPositionSize().height / 2 };
            ball.setPosition(ball_pos);
            ball.setVelocity({ 0,0 });
            ingame_state = PAUSED;
        }
        else if (serve_state == SERVE_STATE::PLAYER_TWO) {
            ball_pos = { paddle_two.getPositionSize().x - paddle_two.getPositionSize().width, paddle_two.getPositionSize().y + paddle_two.getPositionSize().height / 2 };
            ball.setPosition(ball_pos);
            ball.setVelocity({ 0, 0 });
            ingame_state = PAUSED;
        }

        if (ball_forward && serve_state == SERVE_STATE::MIDDLE) {
            ballForward(ball_pos, elapsedTime, speed, paddle_pos2);
        } else if (ball_backward && serve_state == SERVE_STATE::MIDDLE) {
            ballBackward(ball_pos, elapsedTime, speed, paddle_pos1);
        }

        /*
        Handle new serve start
        */
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && serve_state == PLAYER_ONE) {
            ball.setVelocity({ 2, 0 });
            serve_state = SERVE_STATE::MIDDLE;
            ballForward(ball_pos, elapsedTime, speed, paddle_pos2);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && serve_state == PLAYER_TWO) {
            ball.setVelocity({ 2, 0 });
            serve_state = SERVE_STATE::MIDDLE;
            ballBackward(ball_pos, elapsedTime, speed, paddle_pos1);
        }

        /*
        Handle wall collisions
        */
        if (std::floor(ball_pos.y) == court.getDimensions().y + 4 || std::floor(ball_pos.y) == WINDOW_HEIGHT - 8) {
            float ball_y = ball.getVelocity().y;
            ball_y *= -1;
            ball.setVelocity({ ball.getVelocity().x,  ball_y });
        }
        /*
        End handle wall conditions
        */

        /*
        Handle serving states
        */
        

        return GAME_STATE::IN_GAME;

    }

    void Render() {
        court.Render(m_target);
        paddle_one.Render(m_target);
        paddle_two.Render(m_target);
        ball.Render(m_target);
    }
};