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

class Score {
private:
    int player_score;
    Vector2D position;
public:

    Score(int score, Vector2D position) :
        player_score(score),
        position(position)
    {}

    void setScore(int score) {
        this->player_score = score;
    }

    float getScore() {
        return player_score;
    }

    void setScorePosition(Vector2D pos) {
        this->position = pos;
    }

    Vector2D getScorePosition() {
        return position;
    }

    void Render(sf::RenderTarget& target) {
        sf::Font font;
        font.loadFromFile("Quicksand-Bold.otf");
        sf::Text score(std::to_string(player_score), font, 20);
        Vector2D scorePos = getScorePosition();
        score.setPosition({ scorePos.x, scorePos.y });
        score.setFillColor(sf::Color::White);
        target.draw(score);
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
        m_velocity = { 0, 3 };
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
        vel({ 5, 0 }),
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
    float play_count;
    Score player_one_score;
    Score player_two_score;
    bool ball_forward;
    bool ball_backward;
    bool scoreChange;

public:

    PongGame(sf::RenderTarget& target) :
        m_target(target),
        playerOneScore(0),
        playerTwoScore(0),
        maxScore(5),
        court({ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }),
        paddle_one({ 0, WINDOW_HEIGHT / 2 }),
        paddle_two({ WINDOW_WIDTH - 10, WINDOW_HEIGHT / 2 }),
        ingame_state(PLAYING),
        ball_forward(true),
        ball_backward(false),
        play_count(0),
        player_one_score(0, { WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4 }),
        player_two_score(0, { 3 * WINDOW_WIDTH / 4, 3 * WINDOW_HEIGHT / 4 }),
        scoreChange(false)
    {
        // ball.setCallback([this]() {ball_off_one = true; });
    }

    /*
   --------------
   Handle serving state of ball
   --------------
   */
    SERVE_STATE handleServeState(Vector2D ball_pos, Paddle paddle_one, Paddle paddle_two) {
        if (std::floor(ball_pos.x) <= 0 || (ingame_state == PAUSED && ball_pos.x == paddle_two.getPositionSize().x - paddle_two.getPositionSize().width)) {
            return SERVE_STATE::PLAYER_TWO;
        }
        else if (std::floor(ball_pos.x) >= WINDOW_WIDTH || (ingame_state == PAUSED && ball_pos.x == paddle_one.getPositionSize().x + paddle_one.getPositionSize().width)) {
            return SERVE_STATE::PLAYER_ONE;
        }
        return SERVE_STATE::MIDDLE;
    }

    /*
   --------------
   Move ball forward
   --------------
   */
    void ballForward(Vector2D& ball_pos, float elapsedTime, float& speed, RectangleShape& paddle_pos2) {
        ball_pos = { ball_pos.x + ((ball.getVelocity().x * elapsedTime) / 100), ball_pos.y - ((ball.getVelocity().y * elapsedTime) / 100) };
        ball.setPosition(ball_pos);

        if (ball_pos.x >= paddle_pos2.x && (ball_pos.y >= paddle_pos2.y &&
            ball_pos.y <= paddle_pos2.y + paddle_pos2.height) && ingame_state == PLAYING) {
            float rel_intersect_y1 = (paddle_pos2.y + paddle_pos2.height / 2) - ball_pos.y;
            float norm_intersect_y1 = rel_intersect_y1 / (paddle_pos2.height / 2);
            float bounceAngle1 = norm_intersect_y1 * MAXBOUNCEANGLE;
            ball.setVelocity({ speed * cos(bounceAngle1), speed * -sin(bounceAngle1) });
            ball_forward = false;
            ball_backward = true;
        }
    }

    /*
   --------------
   Move ball backward
   --------------
   */
    void ballBackward(Vector2D& ball_pos, float elapsedTime, float& speed, RectangleShape& paddle_pos1) {
        ball_pos = { ball_pos.x - ((ball.getVelocity().x * elapsedTime) / 100), ball_pos.y - ((ball.getVelocity().y * elapsedTime) / 100) };
        ball.setPosition(ball_pos);

        if (ball_pos.x <= paddle_pos1.x + paddle_pos1.width && (ball_pos.y >= paddle_pos1.y &&
            ball_pos.y <= paddle_pos1.y + paddle_pos1.height) && ingame_state == PLAYING) {
            float rel_intersect_y2 = (paddle_pos1.y + paddle_pos1.height / 2) - ball_pos.y;
            float norm_intersect_y2 = rel_intersect_y2 / (paddle_pos1.height / 2);
            float bounceAngle2 = norm_intersect_y2 * MAXBOUNCEANGLE;
            ball.setVelocity({ speed * cos(bounceAngle2), speed * -sin(bounceAngle2) });
            ball_forward = true;
            ball_backward = false;
        }
    }

    /*
   --------------
   Handle ball speed with continous passes
   --------------
   */
    void handleSpeed(Vector2D& ball_pos, SERVE_STATE serve_state, float& speed) {
        if ((std::floor(ball_pos.x) == paddle_one.getPositionSize().x + paddle_one.getPositionSize().width ||
            std::floor(ball_pos.x) == paddle_two.getPositionSize().x - paddle_two.getPositionSize().width) &&
            serve_state == MIDDLE) {
            play_count++;
            ball.setVelocity({ ball.getVelocity().x + play_count / 10, ball.getVelocity().y + play_count / 10 });
        }
    }

    /*
    --------------
    Handle scoring event for players
    --------------
    */
    void handleScore(SERVE_STATE serve_state) {
        sf::Font font;
        font.loadFromFile("Quicksand-Bold.otf");
        int p1 = player_one_score.getScore();
        int p2 = player_two_score.getScore();

        if (serve_state == PLAYER_ONE && scoreChange == false) {
            p1++;
            player_one_score.setScore(p1);
            scoreChange = true;
        }
        else if (serve_state == PLAYER_TWO && scoreChange == false) {
            p2++;
            player_two_score.setScore(p2);
            scoreChange = true;
        }
    }

    // Insert game logic here
    GAME_STATE Update(const float elapsedTime, Vector2D mouse_pos) {
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            ball.setPosition({ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 });
            ball.setVelocity({ 5, 0 });
            paddle_one.setPositionSize({ 0, WINDOW_HEIGHT / 2, 10, 80});
            paddle_two.setPositionSize({ WINDOW_WIDTH - 10, WINDOW_HEIGHT / 2, 10, 80});
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
            paddle_pos1.y -= paddle_one.getVelocity().y * elapsedTime / 100;
            paddle_one.setPositionSize(paddle_pos1);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && (std::floor(paddle_pos1.y + paddle_pos1.height) != WINDOW_HEIGHT)) {
            paddle_pos1.y += paddle_one.getVelocity().y * elapsedTime / 100;
            paddle_one.setPositionSize(paddle_pos1);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && (std::floor(paddle_pos2.y) != 0)) {
            paddle_pos2.y -= paddle_two.getVelocity().y * elapsedTime / 100;
            paddle_two.setPositionSize(paddle_pos2);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && (std::floor(paddle_pos2.y + paddle_pos2.height) != WINDOW_HEIGHT)) {
            paddle_pos2.y += paddle_two.getVelocity().y * elapsedTime / 100;
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
        handleSpeed(ball_pos, serve_state, speed);
        handleScore(serve_state);

        /*
        Restart game if the serve state is either player one or player two and one to score of respective player
        */
        if (serve_state == SERVE_STATE::PLAYER_ONE) {
            ball_pos = { paddle_one.getPositionSize().x + paddle_one.getPositionSize().width, paddle_one.getPositionSize().y + paddle_one.getPositionSize().height / 2 };
            ball.setPosition(ball_pos);
            ball.setVelocity({ 0,0 });
            ingame_state = PAUSED;
            play_count = 0;
        }
        else if (serve_state == SERVE_STATE::PLAYER_TWO) {
            ball_pos = { paddle_two.getPositionSize().x - paddle_two.getPositionSize().width, paddle_two.getPositionSize().y + paddle_two.getPositionSize().height / 2 };
            ball.setPosition(ball_pos);
            ball.setVelocity({ 0, 0 });
            ingame_state = PAUSED;
            play_count = 0;
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
            ball.setVelocity({ 5, 0 });
            serve_state = SERVE_STATE::MIDDLE;
            ingame_state = PLAYING;
            scoreChange = false;
            ballForward(ball_pos, elapsedTime, speed, paddle_pos2);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && serve_state == PLAYER_TWO) {
            ball.setVelocity({ 5, 0 });
            serve_state = SERVE_STATE::MIDDLE;
            ingame_state = PLAYING;
            scoreChange = false;
            ballBackward(ball_pos, elapsedTime, speed, paddle_pos1);
        }

        /*
        Handle wall collisions
        */
        if (ball_pos.y <= court.getDimensions().y + 4 || ball_pos.y >= WINDOW_HEIGHT - 8) {
            float ball_y = ball.getVelocity().y;
            ball_y *= -1;
            ball.setVelocity({ ball.getVelocity().x,  ball_y });
        }
        /*
        End handle wall conditions
        */
        
        return GAME_STATE::IN_GAME;

    }

    void Render() {
        court.Render(m_target);
        paddle_one.Render(m_target);
        paddle_two.Render(m_target);
        ball.Render(m_target);
        player_one_score.Render(m_target);
        player_two_score.Render(m_target);
    }
};