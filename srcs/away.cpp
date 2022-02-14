#include "../irc.hpp"

void irc::away(std::vector<std::string> cmd, int i)
{
    if (cmd.size() == 1)
    {
        clients[i]->setAway(false);
        std::string stringToSend = ":server 305 ";
        stringToSend += clients[i]->getNick();
        stringToSend += " :You are no longer marked as being away\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    clients[i]->setAway(true);
    clients[i]->setAwayMsg(makeStringAfterPrefix(cmd));
    std::string stringToSend = ":server 306 ";
    stringToSend += clients[i]->getNick();
    stringToSend += " :You have been marked as being away\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}