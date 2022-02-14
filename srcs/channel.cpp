#include "../channel.hpp"

void Channel::sendToEverybody(std::string stringToSend, std::vector<Client*> clients)
{
    std::vector<Client*>::iterator cl_it = clients.begin();
    std::vector<Client*>::iterator cl_ite = clients.end();
    while (cl_it != cl_ite)
    {
        std::vector<std::string> clientChannels = (*cl_it)->getChannels();
        std::vector<std::string>::iterator ch_it = clientChannels.begin();
        std::vector<std::string>::iterator ch_ite = clientChannels.end();
        while (ch_it != ch_ite)
        {
            if (*ch_it == this->name)
            {
                send((*cl_it)->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                break;
            }
            ++ch_it;
        }
        ++cl_it;
    }
}

void Channel::removeTopic()
{
    topic.clear();
}

void Channel::setTopic(std::string newTopic)
{
    topic.clear();
    topic = newTopic;
}
std::string Channel::getTopic()
{
    return topic;
}
void Channel::removeClient(std::vector<Channel*> &channels)
{
    clients--;
    if (clients < 1)
        removeChannel(channels, name);        
}
void Channel::addClient()
{
    clients++;
}
int Channel::getClients()
{
    return clients;
}
void Channel::setMaxClients(int q)
{
    maxClients = q;
}

int Channel::getMaxClients()
{
    return maxClients;
}
void Channel::setInviteOnly(bool a)
{
    inviteOnly = a;
}
bool Channel::getInviteOnly()
{
    return inviteOnly;
}
std::string Channel::getOperatorNick()
{
    return operatorNick;
}
void Channel::removePass()
{
    pass.clear();
}
void Channel::setPass(std::string pass)
{
    this->pass = pass;
}
std::string Channel::getPass()
{
    return pass;
}

void Channel::setOperatorNick(std::string nick)
{
    operatorNick = nick;
}
void Channel::setT(bool a)
{
    flagT = a;
}
bool Channel::getT()
{
    return flagT;
}
Channel::Channel(std::string name) : name(name)
{
    inviteOnly = false;
    maxClients = -1;
    clients = 1;
    topic.clear();
    pass.clear();
    flagT = false;
}
void Channel::setName(std::string name)
{
    this->name = name;
}
std::string Channel::getName()
{
    return name;
}
