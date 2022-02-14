#include "../irc.hpp"

std::vector<int> irc::makeChannelIndArray(std::string channelName, std::string nick)
{
    std::vector<int> ind;
    int i = 0;
    std::vector<Client*>::iterator it = clients.begin();
    std::vector<Client*>::iterator ite = clients.end();
    while (it != ite)
    {
        std::vector<std::string> clientChannels = (*it)->getChannels();
        std::vector<std::string>::iterator chanIt = clientChannels.begin();
        std::vector<std::string>::iterator chanIte = clientChannels.end();
        while (chanIt != chanIte)
        {
            if (*chanIt == channelName && (*it)->getNick() != nick)
            {
                ind.push_back(i);
                break;
            }
            ++chanIt;
        }
        ++i;
        ++it;
    }
    return ind;
}
void irc::privmsg(std::vector<std::string> cmd, int i, bool notice)
{
    std::vector<int> ind;
    if (cmd[1][0] != '#' && cmd[1][0] != '&')
    {
        std::vector<std::string>::iterator it_cmd = cmd.begin() + 1;
        std::vector<std::string>::iterator ite_cmd = cmd.end();
        while (it_cmd != ite_cmd && (*it_cmd)[0] != ':')
        {
            ind.push_back(checkExistingNicknames(*it_cmd, clients));
            ++it_cmd;
        }
    }
    else
        ind = makeChannelIndArray(cmd[1], clients[i]->getNick());
    std::string stringToSend = makeStringAfterPrefix(cmd);
    if (stringToSend.empty())
    {
        stringToSend = ":server 412 :No text to send\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    std::string nick = clients[i]->getNick();
    std::vector<int>::iterator it_ind = ind.begin();
    std::vector<int>::iterator ite_ind = ind.end();
    while (it_ind != ite_ind)
    {
        if (*it_ind == -1)
        {
            std::string err = ":server 401 :No such nick/channel\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            ++it_ind;
            continue;
        }
        std::string finalString= ":" + nick + "!";
        std::string tmp = clients[i]->getUsername();
        finalString += tmp;
        finalString += "@127.0.0.1 PRIVMSG ";
        if (cmd[1][0] != '#' && cmd[1][0] != '&')
            tmp = clients[*it_ind]->getNick();
        else
            tmp = cmd[1];
        finalString += tmp;
        finalString += " :";
        finalString += stringToSend;
        finalString += "\n";
        send(clients[*it_ind]->clientSocket, finalString.c_str(), finalString.size(), 0);
        if (clients[*it_ind]->getAway() && !notice)
        {
            std::string str = ":server 301 " + clients[i]->getNick() + " " + clients[*it_ind]->getNick() + " :" + clients[*it_ind]->getAwayMsg() + "\n";
            send(clients[i]->clientSocket, str.c_str(), str.size(), 0);
        }
        ++it_ind;
    }
}