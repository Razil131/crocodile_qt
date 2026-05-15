#pragma once

#include <vector>
#include <tuple>
#include <string>
#include "Player.hpp"

class ChatManager{
private:
    std::vector<std::pair<std::string,std::string>> messages_;
public:
    ChatManager();
    void addMessage(Player ply, std::string message);
    std::vector<std::pair<std::string,std::string>> messages () const {return messages_;}
};