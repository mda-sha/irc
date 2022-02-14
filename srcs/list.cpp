#include "../irc.hpp"

void irc::list(std::vector<std::string> cmd, int i)
{
    std::string stringToSend = ":server 321 " + clients[i]->getNick() + " Channel :Users  Name\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    stringToSend.clear();
    std::vector<Channel*>::iterator ch_it = channels.begin();
    std::vector<Channel*>::iterator ch_ite = channels.end();
    if (cmd.size() > 1)
    {
        while (ch_it != ch_ite)
        {
            if ((*ch_it)->getName() == cmd[1])
            {
                std::string stringToSend = ":server 322 " + clients[i]->getNick() + " " + (*ch_it)->getName() + 
                        " " + std::to_string((*ch_it)->getClients()) + " :[+n] " + (*ch_it)->getTopic() + "\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                stringToSend = ":server 323 " + clients[i]->getNick() + " :End of /LIST\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            }
            ++ch_it;
        }
        stringToSend = ":server 403 " + clients[i]->getNick() + " " + cmd[1] + " :No such channel\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    while (ch_it != ch_ite)
    {
        std::string stringToSend = ":server 322 " + clients[i]->getNick() + " " + (*ch_it)->getName() + 
                " " + std::to_string((*ch_it)->getClients()) + " :[+n] " + (*ch_it)->getTopic() + "\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        ++ch_it;
    }
    stringToSend = ":server 323 " + clients[i]->getNick() + " :End of /LIST\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}