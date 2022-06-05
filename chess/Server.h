#pragma once
#include <SFML/network.hpp>
#include <iostream>
#include "Board.h"

const std::string NO = "n";
const std::string YES = "y";
enum CommandServer {connect_successful };
enum Requests {can_doing, give_name_opponent, move, give_board, give_status_player };

class API {
    CommandServer command;
    std::string data;
public:
    API(CommandServer command, std::string data) : command(command), data(std::move(data)) {}
    CommandServer GetCommand() const { return command; }
    std::string GetData() const { return data; }
    friend sf::Packet & operator<<(sf::Packet & packet, API & api) {
        packet << api.command << api.data;
        return packet;
    }
};
struct PlayerOnServer {
    sf::TcpSocket socket;
    Player * player = nullptr;
};
struct IpAddresses {
    sf::IpAddress ip_public;
    sf::IpAddress ip_local;
};

class Server {
    sf::TcpListener listener;
    IpAddresses addresses;
    unsigned short port;
    PlayerOnServer * move_player = nullptr;
    PlayerOnServer * sleep_player = nullptr;
    Board * board = nullptr;
    bool server_started = false;
public:
    Server();
    const sf::TcpListener & GetListener() { return listener; }
    const IpAddresses & GetIpAddresses() const { return addresses; }
    const unsigned short & GetPort() const { return port; }
    PlayerOnServer & GetMovePlayer() { return * move_player; }
    PlayerOnServer & GetSleepPlayer() { return * sleep_player; }
    Board & GetBoard() { return * board; }
    StatusPlayer GetStatusMovePlayer() { return move_player->player->GetStatus(); }
    sf::Packet GetRequestMovePlayer() const;

    bool IsStarted() const { return server_started; }

    void EditMovePlayer() { std::swap(move_player, sleep_player); }

    Error Move(Pos from, Pos in) { return board->Move(from, in); }

    void Connection();

    sf::Packet PackStatusMovePlayer() const;
    sf::Packet PackNameMovePlayer() const;
    sf::Packet PackMovesMovePlayer(Pos pos) const;
    static sf::Packet PackYes();
    static sf::Packet PackNo();
    sf::Packet PackBoard() const;
};