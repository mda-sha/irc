#include "../irc.hpp"

void irc::part(std::vector<std::string> cmd, int i)
{
    std::string stringToSend;
    if (cmd.size() < 2)
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "USERHOST :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    std::vector<Channel*>::iterator it = channels.begin();
    std::vector<Channel*>::iterator ite = channels.end();
    while (it != ite)
    {
        if ((*it)->getName() == cmd[1])
        {
            if (clients[i]->removeChannel(cmd[1]))
            {
                stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 PART " + cmd[1] + "\n";
                (*it)->sendToEverybody(stringToSend, clients);
                (*it)->removeClient(channels);
            }
            else
            {
                stringToSend = ":server 442 " + cmd[1] + " :You're not on that channel\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            }
            return;
        }
        ++it;
    }
    stringToSend = ":server 403 " + clients[i]->getNick() + " " + cmd[1] + " :No such channel\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}