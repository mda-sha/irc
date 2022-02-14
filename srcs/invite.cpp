#include "../irc.hpp"

void irc::invite(std::vector<std::string> cmd, int i)
{
    std::string stringToSend;
    if (cmd.size() < 3)
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "INVITE :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    if (!clients[i]->checkIfOnChannel(cmd[2]))
    {
        stringToSend = ":server 442 " + cmd[2] + " :You're not on that channel\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    std::vector<Client*>::iterator it_cl = clients.begin();
    std::vector<Client*>::iterator ite_cl = clients.end();
    while (it_cl != ite_cl)
    {
        if ((*it_cl)->getNick() == cmd[1])
        {
            if ((*it_cl)->checkIfOnChannel(cmd[1]))
            {
                stringToSend = ":server 443 " + (*it_cl)->getNick() + " " + cmd[2] + " :is already on channel\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }
            std::vector<Channel*>::iterator it_ch = channels.begin();
            std::vector<Channel*>::iterator ite_ch = channels.end();
            while (it_ch != ite_ch)
            {
                if ((*it_ch)->getName() == cmd[2])
                {
                    if ((*it_ch)->getInviteOnly() && (*it_ch)->getOperatorNick() != clients[i]->getNick())
                    {
                        stringToSend = ":server 482 " + cmd[2] + " :You're not channel operator\n";
                        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                        return;
                    }
                    stringToSend = ":server 341 " + cmd[2] + " " + (*it_cl)->getNick() + "\n";
                    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                    stringToSend.clear();
                    stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 INVITE " + cmd[1] + " :" +  cmd[2] + "\n"; //////отправить всем
                    (*it_ch)->sendToEverybody(stringToSend, clients);
                    send((*it_cl)->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                    return;                        
                }
                ++it_ch;
            }
        }
        ++it_cl;
    }
    stringToSend = ":server 401 " + cmd[1] + " :No such nick/channel\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}