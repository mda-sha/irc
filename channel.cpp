#include "channel.hpp"

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
