#include "../irc.hpp"

void irc::closeAndErase(int i)
{
    close(clients[i]->clientSocket);
    // std::vector<Client*>::iterator it = clients.begin() + i;
    // clients.erase(it);
}

void irc::eraseAll()
{
    std::vector<Channel*>::iterator chIt = channels.begin();
    std::vector<Channel*>::iterator chIte = channels.begin();
    while (chIt != chIte)
    {
        delete(*chIt);
        ++chIt;
    }
    std::vector<Client*>::iterator clIt = clients.begin();
    std::vector<Client*>::iterator clIte = clients.begin();
    while (clIt != clIte)
    {
        close((*clIt)->clientSocket);
        delete(*clIt);
        ++clIt;
    }
    close(sock);
}