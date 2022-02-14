#include "../irc.hpp"

void irc::kick(std::vector<std::string> cmd, int i)
{
    std::string stringToSend;
    if (cmd.size() < 3)
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "KICK :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    if (!clients[i]->checkIfOnChannel(cmd[1]))
    {
        stringToSend = ":server 442 " + cmd[1] + " :You're not on that channel\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    std::vector<Client*>::iterator it_cl = clients.begin();
    std::vector<Client*>::iterator ite_cl = clients.end();        
    while (it_cl != ite_cl)
    {
        if ((*it_cl)->getNick() == cmd[2])
        {
            if ((*it_cl)->checkIfOnChannel(cmd[1]))
            {
                std::vector<Channel*>::iterator it_ch = channels.begin();
                std::vector<Channel*>::iterator ite_ch = channels.end(); 
                while (it_ch != ite_ch)
                {
                    if ((*it_ch)->getName() == cmd[1])
                    {
                        if (clients[i]->getNick() == (*it_ch)->getOperatorNick())
                        {
                            if (cmd.size() == 3)
                                stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 KICK " + cmd[1] + " " +  cmd[2] + " :" + clients[i]->getNick() + "\n"; //////отправить всем
                            else
                                stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 KICK " + cmd[1] + " " +  cmd[2] + " :" + makeStringAfterPrefix(cmd) + "\n"; //////отправить всем
                            (*it_ch)->sendToEverybody(stringToSend, clients);
                            (*it_ch)->removeClient(channels);
                            (*it_cl)->removeChannel(cmd[1]);
                            return;
                        }
                        stringToSend = ":server 482 " + cmd[1] + " :You're not channel operator\n";
                        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                        return;
                    }
                    ++it_ch;
                }
                stringToSend = ":server 403 " + cmd[1] + " :No such channel\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }
        }
        ++it_cl;
    }
    stringToSend = ":server 401 " + cmd[1] + " :No such nick/channel\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}