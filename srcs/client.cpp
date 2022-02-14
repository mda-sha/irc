#include "../client.hpp"

void Client::addToChannel(Channel *channel)
{
    this->clientChannels.push_back(channel->getName());
}

void Client::deleteFromAllChannels(std::vector<Channel*> &channels)
{
    std::vector<std::string>::iterator clientChIt = clientChannels.begin();
    std::vector<std::string>::iterator clientChIte = clientChannels.end();
    while (clientChIt != clientChIte)
    {
        std::vector<Channel*>::iterator it = channels.begin();
        std::vector<Channel*>::iterator ite = channels.end();
        while (it != ite)
        {
            if ((*it)->getName() == *clientChIt)
            {
                (*it)->removeClient(channels);
                clientChannels.erase(clientChIt);
                break;
            }
            ++it;
        }
        ++clientChIt;
    }
}

bool Client::getIsAutorized()
{
    return isAutorized;
}

void Client::checkIfAutorized()
{
    if (password && !realname.empty() &&  !nick.empty() && !username.empty())
    {
        isAutorized = true;
        std::string msg;
        msg = ":server 375 " + nick + " :- server Message of the day -\n";
        send(clientSocket, msg.c_str(), msg.size(), 0);
        msg = ":server 372 " + nick + " : ДАШКА КАКАШКА\n";
        send(clientSocket, msg.c_str(), msg.size(), 0);
        msg = ":server 376 " + nick + " :End of /MOTD command\n";
        send(clientSocket, msg.c_str(), msg.size(), 0);
        std::cout << "new registered user " << nick << std::endl;
    }
}

void Client::setName(std::vector<std::string> cmd)
{
    if (cmd.size() < 5)
    {
        std::string stringToSend = ":server 461 " + nick + " USER :Not enough parameters\n";
        send(clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    username = cmd[1];
    if (cmd[4][0] == ':')
        realname = makeStringAfterPrefix(cmd);
    else
        realname = cmd[4];
}

std::string &Client::getNick() 
{
    return nick;
}

void Client::setNick(std::vector<Client*> clients, int i, std::string nickname)
{
    if ((checkExistingNicknames(nickname, clients)) == -1)
        nick = nickname;
    else
    {
        std::string str = ":server 436 " + nick + " :Nickname is already in use\n";
        send(clients[i]->clientSocket, str.c_str(), str.size(), 0);
    }
}

bool Client::setPassword(std::string g_password, std::string pass)
{
    std::string str;
    if (pass.length() == 0)
    {
        str = ":server 461 PASS :Not enough parameters\n";
        send(clientSocket, str.c_str(), str.size(), 0);
        return 0;
    }
    if (isAutorized)
    {
        str = ":server 462 :You may not reregister\n";
        send(clientSocket, str.c_str(), str.size(), 0);
        return 1;
    }
    if (pass[0] == ':')
        pass = pass.substr(1, pass.size() - 1);
    if (g_password == pass)
        password = true;
    else
        return 0;
    return 1;
}

void Client::makeOper()
{
    oper = true;
}

std::string &Client::getUsername()
{
    return username;
}
std::string &Client::getRealname()
{
    return realname;
}

bool Client::getAway()
{
    return away;
}
void Client::setAway(bool t_f)
{
    away = t_f;
}

std::vector<std::string> Client::getChannels()
{
    return clientChannels;
}

std::string Client::getAwayMsg()
{
    return away_msg;
}

void Client::setAwayMsg(std::string const &str)
{
    away_msg = str;
}

bool Client::removeChannel(std::string name)
{
    std::vector<std::string>::iterator it = clientChannels.begin();
    std::vector<std::string>::iterator ite = clientChannels.end();
    while (it != ite)
    {
        if (name == *it)
        {
            clientChannels.erase(it);
            return true;
        }
    }
    return false;
}

bool Client::checkIfOnChannel(std::string name)
{
    std::vector<std::string>::iterator it = clientChannels.begin();
    std::vector<std::string>::iterator ite = clientChannels.end();
    while (it != ite)
    {
        if (name == *it)
        {
            return true;
        }
        ++it;
    }
    return false;
}

bool Client::getOper()
{
    return oper;
}

bool Client::getPingSent()
{
    return pingSent;
}

void Client::pingFlagsToFalse()
{
    pingFlag = 0;
    pingSent = 0;
}

int Client::checkLastActivity()
{
    std::time_t now = std::time(nullptr);
    if (now - lastActivity > 90 && isAutorized)
    {
        pingFlag = true;
        pingSent = true;
        return 0;
    }        
    if (now - lastActivity > 60 && isAutorized)
    {
        pingFlag = true;
        pingSent = true;
        return 1;
    }
    pingFlag = false;
    return 2;
}

void Client::setLastActivity()
{
    lastActivity = std::time(nullptr);
    pingFlag = false;
}

Client::Client(int sock) : clientSocket(sock)
{
    isAutorized = 0;
    away = 0;
    oper = 0;
    nick.clear();
    realname.clear();
    username.clear();
    pingFlag = 0;
    pingSent = 0;
}