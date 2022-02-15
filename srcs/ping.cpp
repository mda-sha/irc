#include "../irc.hpp"

void irc::pong(int i)
{
    clients[i]->pingFlagsToFalse();
}
void irc::ping()
{
    int index = 0;
    while (index < static_cast<int>(clients.size()))
    {
        if (!clients[index]->getPingSent() && clients[index]->checkLastActivity() == 1)
            send(clients[index]->clientSocket, ":server PING\n" , 14, 0);
        else if (clients[index]->checkLastActivity() == 0)
        {
            clients[index]->deleteFromAllChannels(channels);
            close(clients[index]->clientSocket);
            // std::vector<Client*>::iterator it = clients.begin() + index;
            // clients.erase(it);
        }
        ++index;
    }
}