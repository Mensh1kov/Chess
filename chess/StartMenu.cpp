#include "Display.h"
#include "experimental/filesystem"

namespace fs = std::experimental::filesystem;

void TextureLoader(const std::string path, Textures & pack) {
    for (const auto &file : fs::directory_iterator(path)) {
        sf::Texture * texture = new sf::Texture();
        if (!texture->loadFromFile(file.path().string())) {
            std::cout << "Error load texture!" << std::endl;
        }
        pack[file.path().filename().string()] = texture;
    }
}

sf::TcpSocket * ConnectPlayer(sf::TcpListener & listener) {
    sf::TcpSocket * socket = new sf::TcpSocket;
    while (listener.accept(*socket) != sf::Socket::Done) {
        std::cout << "Connection error!" << std::endl;
    }
    std::cout << "Player connected!" << std::endl;
    return socket;
}

sf::TcpSocket * ConnectLobby() {
    sf::TcpSocket * socket = new sf::TcpSocket;
    std::string ip;
    std::string port;
    sf::Socket::Status status;
    do {
        std::cout << "Enter ip:";
        std::cin >> ip;
        std::cout << "Enter port:";
        std::cin >> port;
        status = socket->connect(ip, std::stoi(port));
    } while (status != sf::Socket::Done);
    std::cout << "Connect successful!" << std::endl;
    return socket;
}

Display * NewGame(sf::RenderWindow * window) {
    Player * player_1 = new Player("test_1", white);
    Player * player_2 = new Player("test_2", black);
    return new Game(window, player_1, player_2);
}

Display * CreateLobby(sf::RenderWindow * window) {
    std::string your_name;
    std::cout << "Enter your name:";
    std::cin >> your_name;

    sf::IpAddress ip_public = sf::IpAddress::getPublicAddress();
    std::cout << "Public ip: " << ip_public << std::endl;

    sf::IpAddress ip_local = sf::IpAddress::getLocalAddress();
    std::cout << "Local ip: " << ip_local << std::endl;
    sf::TcpListener listener;
    if (listener.listen(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cout << "Failed to start the server: server port error!" << std::endl;
    } else {
        std::cout << "Port: " << listener.getLocalPort() << std::endl;
    }
    sf::TcpSocket * socket = ConnectPlayer(listener);
    sf::Packet packet;
    while (socket->receive(packet) != sf::Socket::Done) {
        std::cout << "Get name player error!" << std::endl;
    }
    std::string name_opponent;
    packet >> name_opponent;
    packet.clear();
    packet << your_name;
    std::cout << "Opponent name: " << name_opponent << std::endl;
    if (socket->send(packet) != sf::Socket::Done) {
        std::cout << "Send your_name error!" << std::endl;
    }
    Player * player_1 = new Player(your_name, white);
    Player * player_2 = new Player(name_opponent, black);
    return new OnlineGame(window, socket, player_1, player_2);
}

Display * Connect(sf::RenderWindow * window) {
    sf::TcpSocket * socket;
    sf::Packet packet;
    std::string your_name;
    std::cout << "Enter your name:";
    std::cin >> your_name;
    socket = ConnectLobby();
    packet << your_name;
    if (socket->send(packet) != sf::Socket::Done) {
        std::cout << "Send your name error!" << std::endl;
    }
    std::string opponent_name;
    if (socket->receive(packet) != sf::Socket::Done) {
        std::cout << "Receive opponent name error!" << std::endl;
    }
    packet >> opponent_name;
    std::cout << "Opponent name:" << opponent_name << std::endl;
    Player * player_1 = new Player(your_name, black);
    Player * player_2 = new Player(opponent_name, white);
    return new OnlineGame(window, socket, player_1, player_2);
}

// StartMenu------------------------------------------------

StartMenu::StartMenu(sf::RenderWindow * window) {
    TextureLoader("./Texture/StartMenu/", this->texture_pack);
    float width = 250; // ширина одной кнопоки
    float height = 50;// высота одной кнопки
    float interval = 20; // расстояние мужду кнопками
    float x = 150; // координаты первой кнопки
    float y = 195; // координаты второй кнопки
    float n = 0;
    this->font = new sf::Font;
    if (!this->font->loadFromFile("./Font/calibri.ttf")) {
        std::cout << "Error load font!" << std::endl;
    }
    this->window = window;
    this->new_game = new Button(x, y + n++ * (height + interval), width, height, "New game", sf::Color::White, texture_pack.at("button_new_game.png"));
    this->create_lobby = new Button(x, y + n++ * (height + interval), width, height, "Create lobby", sf::Color::White, texture_pack.at("button_new_game.png"));
    this->connect = new Button(x, y + n++ * (height + interval), width, height, "Connect", sf::Color::White, texture_pack.at("button_new_game.png"));
    this->exit = new Button(x, y + n++ * (height + interval), width, height, "Exit", sf::Color::White, texture_pack.at("button_new_game.png"));
    this->background.setTexture(*texture_pack.at("background_start_menu.png"));
}

void StartMenu::PrintDisplay() const {
    window->draw(background);
    new_game->PrintButton(*window);
    create_lobby->PrintButton(*window);
    connect->PrintButton(*window);
    exit->PrintButton(*window);
}

Display * StartMenu::CheckEvent() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
            return this;
        } else if(event.type == sf::Event::MouseButtonPressed) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                float x = static_cast<float>(sf::Mouse::getPosition(*window).x);
                float y = static_cast<float>(sf::Mouse::getPosition(*window).y);
                if (new_game->IsPresse(x, y)) {
                    next_display = NewGame(window); // должен быть дисплей с вводом имен игроков
                    next_display->SetPastDisplay(this);
                    return next_display;
                } else if (create_lobby->IsPresse(x, y)) {
                    next_display = CreateLobby(window);
                    next_display->SetPastDisplay(this);
                    return next_display;
                } else if (connect->IsPresse(x, y)) {
                    next_display = Connect(window);
                    next_display->SetPastDisplay(this);
                    return next_display;
                } else if (exit->IsPresse(x,y)) {
                    window->close();
                    return this;
                }
            }
        }
    }
    return this;
}

// Game------------------------------------------------------

Game::Game(sf::RenderWindow * window, Player * first, Player * second) {
    TextureLoader("./Texture/Game", texture_pack);
    this->window = window;
    this->font = new sf::Font();
    if (!this->font->loadFromFile("./Font/calibri.ttf")) {
        std::cout << "Game load font error!" << std::endl;
    }
    this->first_player = first;
    this->second_player = second;
    this->game_board = new GameBoard(first_player, second_player);
    this->background.setTexture(*texture_pack.at("background_start_menu.png"));
}

void Game::PrintDisplay() const {
    window->draw(background);
    game_board->PrintPlayingField(*window);
}

Display * Game::CheckEvent() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
            return this;
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                float x = static_cast<float>(sf::Mouse::getPosition(*window).x);
                float y = static_cast<float>(sf::Mouse::getPosition(*window).y);
                game_board->Move(game_board->ToBoardCoor(x, y));
            }
        }
    }
    return this;
}

// Game/GameBoard--------------------------------------------

GameBoard::GameBoard(Player * first, Player * second) {
    TextureLoader("./Texture/GameBoard", this->texture_pack);
    if (first->GetColor() == white) {
        this->start = 7;
        this->end = 0;
        this->shift = -1;
    } else {
        this->start = 0;
        this->end = 7;
        this->shift = 1;
        std::swap(first, second);
    }
    this->size_board = 2 * size_frame + 8 * size_cell;
    this->board = new Board(first, second);
    this->font = new sf::Font();
    if (!this->font->loadFromFile("./Font/calibri.ttf")) {
        std::cout << "Error load font GameBoard!" << std::endl;
    }
    this->text.setFont(*font);
    this->text.setFillColor(sf::Color::White);
    this->text.setCharacterSize(24);

    this->frame = sf::RectangleShape(sf::Vector2f(size_board, size_board));
    this->frame.setTexture(texture_pack.at("square gray dark _png_shadow_512px.png"));

    this->cell = sf::RectangleShape(sf::Vector2f(size_cell, size_cell));
}

void GameBoard::PrintMove(sf::RenderWindow &window) {
    if (choose_piece == nullptr)
        return;

    sf::CircleShape step(10.f);
    step.setOrigin(step.getLocalBounds().width / 2, step.getLocalBounds().height / 2);
    step.setFillColor(sf::Color(61, 240, 58));

    sf::CircleShape cabin(23.f);
    cabin.setOrigin(cabin.getLocalBounds().width / 2, cabin.getLocalBounds().height / 2);
    cabin.setFillColor(sf::Color::Transparent);
    cabin.setOutlineThickness(-6.f);
    cabin.setOutlineColor(sf::Color(61, 240, 58));

    short pos_x;
    short pos_y;
    int n = 0;
    for (auto [key, value] : choose_piece->GetMove()) {
        if (value == NULL_CELL) {
            pos_x = end - key.first * shift;
            pos_y = start + key.second * shift;
            step.setPosition(x + size_frame + size_cell * pos_x + size_cell / 2,
                             y + size_frame + size_cell * pos_y + size_cell / 2);
            window.draw(step);
        } else {
            pos_x = end - value.first * shift;
            pos_y = start + value.second * shift;
            cabin.setPosition(x + size_frame + size_cell * pos_x + size_cell / 2, y + size_frame + size_cell * pos_y + size_cell / 2);
            window.draw(cabin);
        }
    }
}

void GameBoard::PrintPieces(sf::RenderWindow &window) {
    std::pair<float, float> pos(x + size_frame, y + size_frame);
    for (short i = start; i != end + shift; i += shift) {
        for (short j = end; j != start - shift; j -= shift) {
            cell.setPosition(pos.first, pos.second);
            ChessPiece * piece;
            if (board->FindBoard(Pos(j, i))) {
                piece = board->GetBoard().at(Pos(j, i));
            } else {
                std::cout << "There is no such position on the board!" << std::endl;
                piece = nullptr;
            }
            if (piece != nullptr) {
                if (piece->GetColor() == white) {
                    switch (piece->GetLevel()) {
                        case king:
                            cell.setTexture(texture_pack.at("w_king_png_128px.png"));
                            break;
                        case queen:
                            cell.setTexture(texture_pack.at("w_queen_png_128px.png"));
                            break;
                        case rook:
                            cell.setTexture(texture_pack.at("w_rook_png_128px.png"));
                            break;
                        case knight:
                            cell.setTexture(texture_pack.at("w_knight_png_128px.png"));
                            break;
                        case pawn:
                            cell.setTexture(texture_pack.at("w_pawn_png_128px.png"));
                            break;
                        case bishop:
                            cell.setTexture(texture_pack.at("w_bishop_png_128px.png"));
                            break;
                    }
                } else {
                    switch (piece->GetLevel()) {
                        case king:
                            cell.setTexture(texture_pack.at("b_king_png_128px.png"));
                            break;
                        case queen:
                            cell.setTexture(texture_pack.at("b_queen_png_128px.png"));
                            break;
                        case rook:
                            cell.setTexture(texture_pack.at("b_rook_png_128px.png"));
                            break;
                        case knight:
                            cell.setTexture(texture_pack.at("b_knight_png_128px.png"));
                            break;
                        case pawn:
                            cell.setTexture(texture_pack.at("b_pawn_png_128px.png"));
                            break;
                        case bishop:
                            cell.setTexture(texture_pack.at("b_bishop_png_128px.png"));
                            break;
                    }
                }
                window.draw(cell);
            }
            pos.first += size_cell;
        }
        pos.first = x + size_frame;
        pos.second += size_cell;
    }
}

void GameBoard::PrintBoard(sf::RenderWindow &window) {
    std::pair<float, float> pos(x, y);
    frame.setPosition(pos.first, pos.second);
    window.draw(frame);
    pos.first += size_frame;
    pos.second += size_frame;
    int n = 0;
    for (short i = start; i != end + shift; i += shift) {
        for (short j = end; j != start - shift; j -= shift) {
            cell.setPosition(pos.first, pos.second);
            if (choose_piece != nullptr && choose_piece->GetPosition() == Pos(j, i)) {
                cell.setTexture(nullptr);
                cell.setFillColor(sf::Color(61, 240, 58));
            } else if (n % 2 == 0) {
                cell.setTexture(texture_pack.at("square brown light_png_shadow_128px.png"));
            } else {
                cell.setTexture(texture_pack.at("square brown dark_png_shadow_128px.png"));
            }
            window.draw(cell);
            cell.setFillColor(sf::Color::White);
            n++;
            pos.first += size_cell;
        }
        n++;
        pos.first = x + size_frame;
        pos.second += size_cell;
    }
}

void GameBoard::PrintBoardMark(sf::RenderWindow & window) {
    for (int i = start; i != end + shift; i += shift) {
        text.setString(char('1' + (end - i * shift)));
        text.setOrigin(text.getLocalBounds().width / 2, text.getCharacterSize() / 2);
        text.setPosition(x + size_frame + (size_cell / 2) + (size_cell * i), y + size_frame / 2 - 3);
        window.draw(text);
        text.setPosition(x + size_frame + (size_cell / 2) + (size_cell * i), y - size_frame / 2 + size_board - 3);
        window.draw(text);

        text.setString(char('a' + (start + i * shift)));
        text.setOrigin(text.getLocalBounds().width / 2, text.getCharacterSize() / 2);
        text.setPosition(x + size_frame / 2, y + size_frame + size_cell / 2 - 3 + size_cell * i);
        window.draw(text);
        text.setPosition(x - size_frame / 2 + size_board,
                           y + size_frame + size_cell / 2 - 3 + size_cell * i);
        window.draw(text);
    }
}

void GameBoard::PrintWhoMove(sf::RenderWindow &window) {
    text.setString("Move: ");
    text.setOrigin(0, 0);
    text.setPosition(x, y - size_cell);
    window.draw(text);
    if (board->GetMovePlayer().GetColor() == white) {
        cell.setTexture(texture_pack.at("w_king_png_128px.png"));
    } else {
        cell.setTexture(texture_pack.at("b_king_png_128px.png"));
    }
    cell.setOrigin(0, 0);
    cell.setPosition(x + text.getLocalBounds().width, y - size_cell - 13);
    window.draw(cell);
}

void GameBoard::PrintPlayingField(sf::RenderWindow & window) {
    PrintWhoMove(window);
    PrintBoard(window);
    PrintMove(window);
    PrintBoardMark(window);
    PrintPieces(window);
}

Error GameBoard::Move(Pos pos) {
    if (board->FindBoard(pos) && board->GetMovePlayer().FindPiece(board->AtPiece(pos))) {
        choose_piece = board->AtPiece(pos);
        std::cout << "Choose piece: " << choose_piece << std::endl;
    } else if (choose_piece != nullptr) {
        if (board->Move(choose_piece->GetPosition(), pos) == done) {
            choose_piece = nullptr;
            return done;
        } else {
            return in_error;
        }
    } else {
        return from_error;
    }
}

Pos GameBoard::ToBoardCoor(float pos_x, float pos_y) const {
    pos_x = std::floor((pos_x - x - size_frame) / size_cell);
    pos_y = std::floor((pos_y - y - size_frame) / size_cell);
    if (start == 0) {
        pos_x = end - pos_x;
    } else {
        pos_y = start - pos_y;
    }
    std::cout << pos_x << " " << pos_y << std::endl;
    return Pos(pos_x, pos_y);
}


// OnlineGame------------------------------------------------

OnlineGame::OnlineGame(sf::RenderWindow * window, sf::TcpSocket * socket, Player *first, Player *second) {
    TextureLoader("./Texture/Game", texture_pack);
    this->font = new sf::Font();
    if (!this->font->loadFromFile("./Font/calibri.ttf")) {
        std::cout << "Game load font error!" << std::endl;
    }
    this->window = window;
    this->socket = socket;
    this->socket->setBlocking(false);
    this->first_player = first;
    this->second_player = second;
    this->game_board = new GameBoard(first_player, second_player);
    this->background.setTexture(*texture_pack.at("background_start_menu.png"));
}

Display * OnlineGame::CheckEvent() {
    sf::Event event;
    sf::Packet packet;
    if (socket->receive(packet) == sf::Socket::Done) {
        short from_x;
        short from_y;
        short in_x;
        short in_y;
        packet >> from_x >> from_y >> in_x >> in_y;
        std::cout << from_x << " " << from_y << " " << in_x << " " << in_y << std::endl;
        game_board->GetBoard()->Move(Pos(from_x, from_y), Pos(in_x, in_y));
    }

    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
            return this;
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                if (first_player->GetColor() == game_board->GetBoard()->GetMovePlayer().GetColor()) {
                    float x = static_cast<float>(sf::Mouse::getPosition(*window).x);
                    float y = static_cast<float>(sf::Mouse::getPosition(*window).y);
                    Pos pos_in = game_board->ToBoardCoor(x, y);
                    if (game_board->GetChoosePiece() != nullptr) {
                        Pos pos = game_board->GetChoosePiece()->GetPosition();
                        packet << pos.first << pos.second;
                    }
                    if (game_board->Move(pos_in) == done) {
                        packet << pos_in.first << pos_in.second;
                        if (socket->send(packet) != sf::Socket::Done) {
                            std::cout << "Send move error!" << std::endl;
                        }
                    }
                }
            }
        }
    }
    return this;
}