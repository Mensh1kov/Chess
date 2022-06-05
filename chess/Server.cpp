#include "Server.h"

Server::Server() {
    std::cout << "Starting the server..." << std::endl;

    this->addresses.ip_public = sf::IpAddress::getPublicAddress();
    std::cout << "Public ip: " << addresses.ip_public << std::endl;

    this->addresses.ip_local = sf::IpAddress::getLocalAddress();
    std::cout << "Local ip: " << addresses.ip_local << std::endl;

    if (this->listener.listen(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cout << "Failed to start the server: server port error!" << std::endl;
    } else {
        this->port = listener.getLocalPort();
        this->server_started = true;
        std::cout << "Port: " << port << std::endl;
        std::cout << "The server has started!" << std::endl;
    }
}

void ConnectPlayer(sf::TcpListener & listener, PlayerOnServer * player_serv, Color color) {
    while (listener.accept(player_serv->socket) != sf::Socket::Done) {
        std::cout << "Connection error!" << std::endl;
    }
    std::cout << "Player connected!" << std::endl;

    sf::Packet packet;
    while (player_serv->socket.receive(packet) != sf::Socket::Done) {
        std::cout << "Get name player error!" << std::endl;
    }
    std::string name;
    packet >> name;
    player_serv->player = new Player(name, color);
}

void Server::Connection() {
    ConnectPlayer(listener, move_player, white);
    ConnectPlayer(listener, sleep_player, black);
    this->board = new Board(move_player->player, sleep_player->player);
}

sf::Packet Server::GetRequestMovePlayer() const {
    sf::Packet packet;
    while (move_player->socket.receive(packet) != sf::Socket::Done) {
        std::cout << "Get request error!" << std::endl;
    }
    return packet;
}

sf::Packet Server::PackBoard() const {
    sf::Packet packet;
    std::string packed_board;

    if (move_player->player->GetColor() == white)
        packed_board += "w";
    else
        packed_board = "b";
    for (auto [pos, piece] : (*board).GetBoard()) {
        packed_board += std::to_string(pos.first);
        packed_board += std::to_string(pos.second);
        if (piece == nullptr) {
            packed_board += NO;
            continue;
        }
        else if (piece->GetColor() == white)
            packed_board += "w";
        else if (piece->GetColor() == black)
            packed_board += "b";
        switch (piece->GetLevel()) {
            case king:
                packed_board += "K";
                break;
            case queen:
                packed_board += "q";
                break;
            case rook:
                packed_board += "r";
                break;
            case bishop:
                packed_board += "b";
                break;
            case knight:
                packed_board += "k";
                break;
            case pawn:
                packed_board += "p";
                break;
        }
    }
    packet << packed_board;
    return packet;
}

sf::Packet Server::PackMovesMovePlayer(Pos pos) const {
    sf::Packet packet;
    std::string moves;
    ChessPiece * piece = board->GetBoard().at(pos);
    if (move_player->player->FindPiece(piece)) {
        for (auto m : piece->GetMove()) {
            moves += std::to_string(m.first.first);
            moves += std::to_string(m.second.second);
        }
    } else {
        moves = NO;
    }
    packet << moves;
    return packet;
}

sf::Packet Server::PackNameMovePlayer() const {
    sf::Packet packet;
    std::string name;
    if (move_player->player != nullptr) {
        name = move_player->player->GetName();
    } else {
        name = NO;
    }
    packet << name;
    return  packet;
}

sf::Packet Server::PackStatusMovePlayer() const {
    sf::Packet packet;
    StatusPlayer status;
    if (move_player->player != nullptr) {
        status = move_player->player->GetStatus();
    } else {
        status = uncertainties;
    }
    packet << status;
    return packet;
}

sf::Packet Server::PackYes() {
    sf::Packet packet;
    packet << YES;
    return packet;
}

sf::Packet Server::PackNo() {
    sf::Packet packet;
    packet << NO;
    return packet;
}
