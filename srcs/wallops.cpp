#include "../irc.hpp"

void irc::wallops(std::vector<std::string> cmd, int i)
{
    std::string stringToSend;
    if (cmd.size() < 2)
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "WALLOPS :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    stringToSend = ":" + clients[i]->getNick() + " WALLOPS :";
    for (int i = 1; i < static_cast<int>(cmd.size()); i++)
        stringToSend += cmd[i] + " ";
    stringToSend += "\n";
    std::vector<Client*>::iterator it = clients.begin();
    std::vector<Client*>::iterator ite = clients.end();
    while (it != ite)
    {
        if ((*it)->getOper())
            send((*it)->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        ++it;
    }
}