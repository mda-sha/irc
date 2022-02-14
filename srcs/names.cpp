#include "../irc.hpp"

void irc::names(std::vector<std::string> cmd, int i)
{
    std::string stringToSend;
    std::vector<Channel*>::iterator ch_it = channels.begin();
    std::vector<Channel*>::iterator ch_ite = channels.end();
    if (cmd.size() > 1)
    {
        while (ch_it != ch_ite)
        {
            if ((*ch_it)->getName() == cmd[1])
            {
                stringToSend = makeUsersInChannelString((*ch_it)->getName(), i);
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                stringToSend.clear();
                stringToSend = ":server 366 " + clients[i]->getNick() + " " + cmd[1] + " :End of /NAMES list\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }
            ++ch_it;
        }
        stringToSend = ":server 403 " + clients[i]->getNick() + " " + cmd[1] + " :No such channel\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    while (ch_it != ch_ite)
    {
        stringToSend = makeUsersInChannelString((*ch_it)->getName(), i);
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        stringToSend.clear();
        ++ch_it;
    }
    stringToSend = ":server 366 " + clients[i]->getNick() + " * :End of /NAMES list\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}