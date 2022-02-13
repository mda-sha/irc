#include "client.hpp"

void Client::addToChannel(Channel *channel) {
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