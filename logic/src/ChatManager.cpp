#include "ChatManager.hpp"

ChatManager::ChatManager(){

}

void ChatManager::addMessage(Player ply, std::string message){
    messages_.emplace_back(ply.name(), message);
}