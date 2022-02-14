#include "../irc.hpp"

std::vector<std::string> irc::splitString(std::string buf)
{
    std::vector<std::string> strings;
    int i = 0;
    int len = buf.length();
    std::size_t tmp;
    while (i < len)
    {
        tmp = buf.find(' ', i);
        if (tmp == std::string::npos)
        {
            std::string newString = buf.substr(i, len - i);
            strings.push_back(newString);
            break;
        }
        std::string newString = buf.substr(i, tmp - i);
        if (newString.length() > 0)
            strings.push_back(newString);
        i = tmp + 1;
    }
    return strings;
}

void irc::getRegistered(std::vector<std::string> cmd, int i)
{
    if (cmd[0] == "PASS")
    {
        if (!clients[i]->setPassword(g_password, cmd[1]))
            return;
    }
    else
    {
        if (!clients[i]->getPass())
            closeAndErase(i);
    }
    if (cmd[0] == "USER")
    {
        if (!clients[i]->getPass())
            closeAndErase(i);
        else
            clients[i]->setName(cmd);
    }
    if (cmd[0] == "NICK" && clients[i]->getPass())
    {
        if (!clients[i]->getPass())
            closeAndErase(i);
        else
            clients[i]->setNick(clients, i, cmd[1]);
    }
    clients[i]->checkIfAutorized();
}
    