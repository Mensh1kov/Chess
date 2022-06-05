#pragma once
#include "SFML/network.hpp"
#include "iostream"
#include "Board.h"
#include "Button.h"
#include "math.h"

using Textures = std::map<std::string, sf::Texture *>;

class GameBoard {
    Textures texture_pack;
    sf::Font * font = nullptr;
    float x = 40;
    float y = 140;
    float size_frame = 35;
    float size_cell = 50;
    float size_board;
    short start;
    short end;
    short shift;
    sf::RectangleShape frame;
    sf::RectangleShape cell;
    sf::Text text;
    ChessPiece * choose_piece = nullptr;
    Board * board;
    void PrintMove(sf::RenderWindow & window);
    void PrintBoard(sf::RenderWindow & window);
    void PrintBoardMark(sf::RenderWindow & window);
    void PrintPieces(sf::RenderWindow & window);
    void PrintWhoMove(sf::RenderWindow & window);
public:
    ChessPiece * GetChoosePiece() const { return choose_piece; }
    Pos ToBoardCoor(float x, float y) const;
    GameBoard(Player * first, Player * second);
    void PrintPlayingField(sf::RenderWindow & window); // PlayingField
    Error Move(Pos pos);
    Board * GetBoard() { return board; }
};

class Display {
protected:
    Textures texture_pack;
    sf::Font * font = nullptr;
    sf::RenderWindow * window = nullptr;
    Display * next_display = nullptr;
    Display * past_display = nullptr;
public:
    sf::RenderWindow & GetWindow() const { return *window; }
    Display * GetNextDisplay() { return next_display; }
    Display * SetPastDisplay(Display * display) { past_display = display; }
    virtual void PrintDisplay() const = 0;
    virtual Display * CheckEvent() = 0;
};

class StartMenu : public Display {
    Button * new_game;
    Button * create_lobby;
    Button * connect;
    Button * exit;
    sf::Sprite background;
public:
    StartMenu(sf::RenderWindow * window);
    void PrintDisplay() const;
    Display * CheckEvent();
};

class Game : public Display {
protected:
    sf::Sprite background;
    GameBoard * game_board;
    Player * first_player;
    Player * second_player;
public:
    Game() {};
    Game(sf::RenderWindow * window, Player * first, Player * second); // нужно передавать двух игроков
    void PrintDisplay() const;
    Display * CheckEvent();
};

class OnlineGame : public Game {
    sf::TcpSocket * socket;
public:
    OnlineGame(sf::RenderWindow * window, sf::TcpSocket * socket, Player * first, Player * second);
    Display * CheckEvent();
};
