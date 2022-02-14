#include "../irc.hpp"

void irc::kill(std::vector<std::string> cmd, int i)
{
    std::string stringToSend;
    if (!clients[i]->getOper())
    {
        stringToSend = ":server 481 :Permission Denied- You're not an IRC operator\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    if (cmd.size() < 2)
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "KILL :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    if (!clients[i]->getOper())
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "KILL :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    std::vector<Client*>::iterator it = clients.begin();
    std::vector<Client*>::iterator ite = clients.end();
    while (it != ite)
    {
        if ((*it)->getNick() == cmd[1])
        {
            (*it)->deleteFromAllChannels(channels);
            close((*it)->clientSocket);
            clients.erase(it);
            return;
        }
        ++it;
    }
    stringToSend = ":server 401 " + cmd[1] + " :No such nick/channel\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}