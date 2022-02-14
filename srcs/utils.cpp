#include "../irc.hpp"

void removeChannel(std::vector<Channel*> &channels, std::string channelName)
{
    std::vector<Channel*>::iterator it = channels.begin();
    std::vector<Channel*>::iterator ite = channels.end();
    while (it != ite)
    {
        if ((*it)->getName() == channelName)
        {
            channels.erase(it);
            break;
        }
        ++it;
    }
}

std::string makeStringAfterPrefix(std::vector<std::string> cmd)
{
    std::vector<std::string>::iterator it = cmd.begin();
    std::vector<std::string>::iterator ite = cmd.end();
    while (it != ite)
    {
        if ((*it)[0] == ':')
            break;
        it++;
    }
    (*it).erase(0, 1);
    std::string ret;
    ret.erase();
    while (it != ite)
    {
        ret.append(*it);
        if (it + 1 != ite)
            ret.append(" ");
        it++;
    }
    return ret;
}

int checkExistingNicknames(std::string const &nickname, std::vector<Client*>clients)
{
    int i = 0;
    int len = clients.size();
    while (i < len)
    {
        if (clients[i]->getNick() == nickname)
            return i;
        i++;
    }
    return -1;
}