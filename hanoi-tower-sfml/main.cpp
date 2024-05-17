#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <string>

const int CELL_SIZE = 40;
const int FONT_SIZE = 24;
const int WIN_COUNT = 5;
const std::string FONT_PATH = "C:/Windows/Fonts/arial.ttf";

int FIELD_WIDTH = 20;
int FIELD_HEIGHT = 15;

enum class GameState {
    MENU,
    ABOUT,
    GAME,
    EXIT
};

enum class Player {
    NONE,
    RED,
    BLUE
};

struct Cell {
    Player owner = Player::NONE;
};

class TicTacToe {
public:
    TicTacToe()
        : window(sf::VideoMode(800, 600), "Tic-Tac-Toe"), state(GameState::MENU), current_player(Player::RED), game_won(false) {
        if (!font.loadFromFile(FONT_PATH)) {
            std::cerr << "Failed to load font" << std::endl;
            exit(1);
        }

        setup_menu();
        setup_about();
    }

    void run() {
        while (window.isOpen()) {
            process_events();
            update();
            render();
        }
    }

private:
    sf::RenderWindow window;
    sf::Font font;
    GameState state;
    Player current_player;
    std::vector<std::vector<Cell>> board;
    sf::Text status_text;
    sf::Text menu_text[3];
    sf::Text about_text;
    int menu_selected_index = 0;
    sf::Clock restart_clock;
    bool game_won;

    void setup_menu() {
        menu_text[0].setFont(font);
        menu_text[0].setString("Start");
        menu_text[0].setCharacterSize(FONT_SIZE);
        menu_text[0].setFillColor(sf::Color::Red);
        menu_text[0].setPosition(350, 200);

        menu_text[1].setFont(font);
        menu_text[1].setString("About");
        menu_text[1].setCharacterSize(FONT_SIZE);
        menu_text[1].setFillColor(sf::Color::Black);
        menu_text[1].setPosition(350, 250);

        menu_text[2].setFont(font);
        menu_text[2].setString("Exit");
        menu_text[2].setCharacterSize(FONT_SIZE);
        menu_text[2].setFillColor(sf::Color::Black);
        menu_text[2].setPosition(350, 300);
    }

    void setup_about() {
        about_text.setFont(font);
        about_text.setString("Tic-Tac-Toe game for two players.\nPress ESC to go back to the menu.");
        about_text.setCharacterSize(FONT_SIZE);
        about_text.setFillColor(sf::Color::Black);
        about_text.setPosition(100, 200);
    }

    void setup_game() {
        board.clear();
        board.resize(FIELD_HEIGHT, std::vector<Cell>(FIELD_WIDTH));

        current_player = Player::RED;
        game_won = false;
        status_text.setFont(font);
        status_text.setCharacterSize(FONT_SIZE);
        status_text.setFillColor(sf::Color::Black);
        update_status_text();
    }

    void update_status_text() {
        if (current_player == Player::RED) {
            status_text.setString("Player Red's turn");
        }
        else {
            status_text.setString("Player Blue's turn");
        }
        status_text.setPosition(10, 10);
    }

    void process_events() {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                handle_key_press(event.key.code);
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handle_click(event.mouseButton.x, event.mouseButton.y);
                }
                break;
            default:
                break;
            }
        }
    }

    void handle_key_press(sf::Keyboard::Key key) {
        if (state == GameState::MENU) {
            if (key == sf::Keyboard::Up) {
                menu_selected_index = (menu_selected_index + 2) % 3;
                update_menu_colors();
            }
            else if (key == sf::Keyboard::Down) {
                menu_selected_index = (menu_selected_index + 1) % 3;
                update_menu_colors();
            }
            else if (key == sf::Keyboard::Enter) {
                handle_menu_selection();
            }
        }
        else if (state == GameState::ABOUT && key == sf::Keyboard::Escape) {
            state = GameState::MENU;
        }
        else if (state == GameState::GAME && key == sf::Keyboard::Escape) {
            state = GameState::MENU;
        }
    }

    void handle_click(int x, int y) {
        if (state == GameState::MENU) {
            for (int i = 0; i < 3; ++i) {
                if (menu_text[i].getGlobalBounds().contains(x, y)) {
                    menu_selected_index = i;
                    handle_menu_selection();
                    break;
                }
            }
        }
        else if (state == GameState::GAME && !game_won) {
            int row = y / CELL_SIZE;
            int col = x / CELL_SIZE;

            if (row >= 0 && row < board.size() && col >= 0 && col < board[0].size() && board[row][col].owner == Player::NONE) {
                board[row][col].owner = current_player;
                if (check_win(row, col)) {
                    status_text.setString((current_player == Player::RED ? "Player Red" : "Player Blue") + std::string(" won!"));
                    game_won = true;
                    restart_clock.restart();
                }
                else {
                    current_player = (current_player == Player::RED) ? Player::BLUE : Player::RED;
                    update_status_text();
                }
            }
        }
    }

    void handle_menu_selection() {
        switch (menu_selected_index) {
        case 0: // Start
            state = GameState::GAME;
            setup_game();
            break;
        case 1: // About
            state = GameState::ABOUT;
            break;
        case 2: // Exit
            window.close();
            break;
        }
    }

    bool check_win(int row, int col) {
        Player player = board[row][col].owner;
        return check_direction(row, col, 1, 0, player) ||
            check_direction(row, col, 0, 1, player) ||
            check_direction(row, col, 1, 1, player) ||
            check_direction(row, col, 1, -1, player);
    }

    bool check_direction(int row, int col, int dr, int dc, Player player) {
        int count = 1;
        for (int i = 1; i < WIN_COUNT; ++i) {
            int r = row + dr * i, c = col + dc * i;
            if (r < 0 || r >= board.size() || c < 0 || c >= board[0].size() || board[r][c].owner != player) break;
            ++count;
        }
        for (int i = 1; i < WIN_COUNT; ++i) {
            int r = row - dr * i, c = col - dc * i;
            if (r < 0 || r >= board.size() || c < 0 || c >= board[0].size() || board[r][c].owner != player) break;
            ++count;
        }
        return count >= WIN_COUNT;
    }

    void update_menu_colors() {
        for (int i = 0; i < 3; ++i) {
            menu_text[i].setFillColor(i == menu_selected_index ? sf::Color::Red : sf::Color::Black);
        }
    }

    void update() {
        if (state == GameState::GAME && game_won && restart_clock.getElapsedTime().asSeconds() >= 3) {
            setup_game();
        }
    }

    void render() {
        window.clear(sf::Color::White);

        if (state == GameState::MENU) {
            for (const auto& text : menu_text) {
                window.draw(text);
            }
        }
        else if (state == GameState::ABOUT) {
            window.draw(about_text);
        }
        else if (state == GameState::GAME) {
            draw_board();
            window.draw(status_text);
        }

        window.display();
    }

    void draw_board() {
        sf::RectangleShape line(sf::Vector2f(CELL_SIZE * board[0].size(), 1));
        line.setFillColor(sf::Color::Black);

        for (int i = 1; i < board.size(); ++i) {
            line.setPosition(0, i * CELL_SIZE);
            window.draw(line);
        }

        line.setSize(sf::Vector2f(1, CELL_SIZE * board.size()));
        for (int i = 1; i < board[0].size(); ++i) {
            line.setPosition(i * CELL_SIZE, 0);
            window.draw(line);
        }

        sf::CircleShape circle(CELL_SIZE / 2 - 2);
        circle.setOutlineThickness(2);
        circle.setOutlineColor(sf::Color::Blue);
        circle.setFillColor(sf::Color::Transparent);

        sf::RectangleShape cross1(sf::Vector2f(CELL_SIZE - 4, 4));
        cross1.setFillColor(sf::Color::Red);
        sf::RectangleShape cross2 = cross1;
        cross2.setRotation(90);

        for (int r = 0; r < board.size(); ++r) {
            for (int c = 0; c < board[r].size(); ++c) {
                if (board[r][c].owner == Player::RED) {
                    cross1.setPosition(c * CELL_SIZE + 2, r * CELL_SIZE + CELL_SIZE / 2 - 2);
                    cross2.setPosition(c * CELL_SIZE + CELL_SIZE / 2 - 2, r * CELL_SIZE + 2);
                    window.draw(cross1);
                    window.draw(cross2);
                }
                else if (board[r][c].owner == Player::BLUE) {
                    circle.setPosition(c * CELL_SIZE + 2, r * CELL_SIZE + 2);
                    window.draw(circle);
                }
            }
        }
    }
};

int main() {
    TicTacToe game;
    game.run();
    return 0;
}
