#include "client.hpp"

void Client::addToChannel(Channel *channel) {
    this->clientChannels.push_back(channel->getName());
}