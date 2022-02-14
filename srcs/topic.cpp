#include "../irc.hpp"

void irc::topic(std::vector<std::string> cmd, int i)
{
    std::string stringToSend;
    if (cmd.size() < 2)
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "TOPIC :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    if (!clients[i]->checkIfOnChannel(cmd[1]))
    {
        stringToSend = ":server 442 " + cmd[1] + " :You're not on that channel\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    std::string newTopic;
    std::vector<Channel*>::iterator it_ch = channels.begin();
    std::vector<Channel*>::iterator ite_ch = channels.end(); 
    while (it_ch != ite_ch)
    {
        if ((*it_ch)->getName() == cmd[1])
        {
            if (cmd.size() > 2)
            {
                if ((*it_ch)->getT() && (*it_ch)->getOperatorNick() != clients[i]->getNick())
                {
                    stringToSend = ":server 482 " + cmd[2] + " :You're not channel operator\n";
                    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                    return;
                }
                if (cmd.size() > 3 || (cmd.size() == 3 && cmd[2][0] == ':'))
                    newTopic = makeStringAfterPrefix(cmd);
                else
                    newTopic = cmd[2];
                (*it_ch)->setTopic(newTopic);
                stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 TOPIC "
                         +  cmd[1] + " :" + newTopic + "\n";
                (*it_ch)->sendToEverybody(stringToSend, clients);
                return;
            }
            else
            {
                std::string topic = (*it_ch)->getTopic();
                if (topic.empty())
                    stringToSend = ":server 331 " + clients[i]->getNick() + " " + cmd[1] + " :No topic is set\n";
                else
                    stringToSend = ":server 332 " + clients[i]->getNick() + " " + cmd[1] + " :" + topic + "\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }
        }
        ++it_ch;
    }
    stringToSend = ":server 401 " + cmd[1] + " :No such nick/channel\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}