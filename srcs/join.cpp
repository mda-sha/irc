#include "../irc.hpp"

bool irc::checkChannelName(std::string name)
{
    if (name[0] != '&' && name[0] != '#')
        return false;
    if (name.find(' ') != std::string::npos || name.find(',') != std::string::npos)
        return false;
    return true;
}
std::string irc::makeUsersInChannelString(std::string channelName, int i)
{
    std::string str = ":server 353 " + clients[i]->getNick() + " = " + channelName + " :";
    std::vector<Client*>::iterator it = clients.begin();
    std::vector<Client*>::iterator ite = clients.end();
    while (it != ite)
    {
        std::vector<std::string> clientChannels = (*it)->getChannels();
        std::vector<std::string>::iterator cl_ch_it = clientChannels.begin();
        std::vector<std::string>::iterator cl_ch_ite = clientChannels.end();
        while (cl_ch_it != cl_ch_ite)
        {
            if (*cl_ch_it == channelName)
            {
                std::vector<Channel*>::iterator ch_it = channels.begin();
                std::vector<Channel*>::iterator ch_ite = channels.end();
                while (ch_it != ch_ite)
                {
                    if ((*ch_it)->getOperatorNick() == (*it)->getNick() && channelName == (*ch_it)->getName())
                    {
                        str+= "@";
                        break;
                    }
                    ++ch_it;
                }
                str += (*it)->getNick();
                str+= " ";
                break;
            }
            ++cl_ch_it;
        }
        ++it;
    }
    str.erase(str.length() - 1, 1);
    str += "\n";
    return str;
}
void irc::sendMsgAfterJoin(std::vector<std::string> cmd, int i, Channel newChannel)
{
    std::string stringToSend;
    stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 JOIN :" +  newChannel.getName() + "\n";/////отправить всем на канале
    newChannel.sendToEverybody(stringToSend, clients);
    stringToSend.clear();
    std::string topic = newChannel.getTopic();
    if (topic.empty())
        stringToSend = ":server 331 " + clients[i]->getNick() + " " + cmd[1] + " :No topic is set\n";
    else
        stringToSend = ":server 332 " + clients[i]->getNick() + " " + cmd[1] + " :" + topic + "\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    stringToSend.clear();
    stringToSend = makeUsersInChannelString(cmd[1], i);
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    stringToSend.clear();
    stringToSend = ":server 366 " + clients[i]->getNick() + " " + cmd[1] + " :End of /NAMES list\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}


void irc::join(std::vector<std::string> cmd, int i)
{
    std::string stringToSend;
    if (cmd.size() == 1)
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "JOIN :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    std::vector<Channel*>::iterator it = channels.begin();
    std::vector<Channel*>::iterator ite = channels.end();
    while (it != ite)
    {
        if ((*it)->getName() == cmd[1])
        {
            if ((*it)->getInviteOnly())
            {
                stringToSend = ":server 473 " + cmd[1] + " :Cannot join channel (+i)\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }
            if ((*it)->getMaxClients() != -1 && (*it)->getClients() == (*it)->getMaxClients())
            {
                stringToSend = ":server 471 " + cmd[1] + " :Cannot join channel (+l)\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }
            std::string pass = (*it)->getPass();
            if (!pass.empty() && cmd[2] != pass)
            {
                stringToSend = ":server 475 " + cmd[1] + " :Cannot join channel (+k)\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }
            if (!clients[i]->checkIfOnChannel((*it)->getName()))
            {
                clients[i]->addToChannel(*it);
                (*it)->addClient();
                sendMsgAfterJoin(cmd, i, **it);
            }
            return;
        }
        ++it;
    }
    if (it == ite)
    {
        if (!checkChannelName(cmd[1]))
        {
            stringToSend = ":server 403 " + clients[i]->getNick() + " " + cmd[1] + " :No such channel\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            return;
        }
        Channel *newChannel = new Channel(cmd[1]);
        newChannel->setOperatorNick(clients[i]->getNick());
        clients[i]->addToChannel(newChannel);
        channels.push_back(newChannel);
        sendMsgAfterJoin(cmd, i, *newChannel);
        std::cout << "new channel " << cmd[1] << " created" << std::endl;
    }
}