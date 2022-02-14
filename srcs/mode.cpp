#include "../irc.hpp"

void irc::modeT(std::vector<std::string> cmd, int i, std::string const &charFrst, std::vector<Channel*>::iterator it_ch)
{
    std::string stringToSend;
    if (cmd[2][0] == '-')
    {
        (*it_ch)->setT(false);
        stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 MODE " +  (*it_ch)->getName() + " " + cmd[2] + "\n";
        (*it_ch)->sendToEverybody(stringToSend, clients);
    }
    else if (cmd[2][0] == '+')
    {
        (*it_ch)->setT(true);
        stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 MODE " +  (*it_ch)->getName() + " " + cmd[2] + "\n";
        (*it_ch)->sendToEverybody(stringToSend, clients);
    }
    else
    {
        stringToSend = ":server 472 " + charFrst + " :is unknown mode char to me\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    }
}
void irc::modeL(std::vector<std::string> cmd, int i, std::string const &charFrst, std::vector<Channel*>::iterator it_ch)
{
    std::string stringToSend;
    int n = atoi(cmd[3].c_str());
    if (n == 0)
        n = -1;
    if (cmd[2][0] == '-')
    {
        (*it_ch)->setMaxClients(-1);
        stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 MODE " +  (*it_ch)->getName() + " " + cmd[2] + "\n";
        (*it_ch)->sendToEverybody(stringToSend, clients);
    }
    else if (cmd[2][0] == '+')
    {
        if (cmd.size() < 4)
        {
            stringToSend = ":server 461 " + clients[i]->getNick() + "MODE :Not enough parameters\n";
            send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
            return;
        }            
        (*it_ch)->setMaxClients(n);
        stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 MODE " +  (*it_ch)->getName() + " " + cmd[2] + "\n";
        (*it_ch)->sendToEverybody(stringToSend, clients);
    }
    else
    {
        stringToSend = ":server 472 " + charFrst + " :is unknown mode char to me\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    }
}
void irc::modeK(std::vector<std::string> cmd, int i, std::string const &charFrst, std::vector<Channel*>::iterator it_ch)
{
    std::string stringToSend;
    if (cmd.size() < 4)
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "MODE :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    if (cmd[2][0] == '-')
    {
        (*it_ch)->removePass();
        stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 MODE " +  (*it_ch)->getName() + " " + cmd[2] + "\n";
        (*it_ch)->sendToEverybody(stringToSend, clients);
    }
    else if (cmd[2][0] == '+')
    {
        (*it_ch)->setPass(cmd[3]);
        stringToSend = ":" + clients[i]->getNick() + "!" + clients[i]->getUsername() + "@127.0.0.1 MODE " +  (*it_ch)->getName() + " " + cmd[2] + "\n";
        (*it_ch)->sendToEverybody(stringToSend, clients);
    }
    else
    {
        stringToSend = ":server 472 " + charFrst + " :is unknown mode char to me\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
    }
}
void irc::mode(std::vector<std::string> cmd, int i)
{
    std::string stringToSend;
    stringToSend.clear();
    if (cmd.size() < 3)
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "MODE :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    std::vector<Channel*>::iterator it_ch = channels.begin();
    std::vector<Channel*>::iterator ite_ch = channels.end();
    std::string charFrst;
    std::string charScnd;
    charFrst.insert(0, 1, cmd[2][0]);
    charScnd.insert(0, 1, cmd[2][0]);
    while (it_ch != ite_ch)
    {
        if ((*it_ch)->getName() == cmd[1])
        {
            if (cmd[2].length() < 2)
            {
                stringToSend = ":server 472 " + charFrst + " :is unknown mode char to me\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }
            if ((*it_ch)->getOperatorNick() != clients[i]->getNick())
            {
                stringToSend = ":server 482 " + cmd[1] + " :You're not channel operator\n";
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }
            if (cmd[2][1] == 't')
            {
                modeT(cmd, i, charFrst, it_ch);
                return;
            }
            else if (cmd[2][1] == 'l')
            {
                modeL(cmd, i, charFrst, it_ch);
                return;
            }
            else if (cmd[2][1] == 'k')
            {
                modeK(cmd, i, charFrst, it_ch);
                return;                   
            }
            else
                stringToSend = ":server 472 " + charScnd + " :is unknown mode char to me\n";
            if (!stringToSend.empty())
            {
                send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                return;
            }               
        }
        ++it_ch;
    }
    stringToSend = ":server 401 " + cmd[1] + " :No such nick/channel\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}