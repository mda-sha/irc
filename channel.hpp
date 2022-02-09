#ifndef CHANNEL
#define CHANNEL

// class Client;
#include "client.hpp"

class Channel
{
private:
    std::string name;
    bool inviteOnly;
    std::string pass;
    std::string topic;
    std::string operatorNick;
    int maxClients;
    int clients;

public:
    std::string getName() {return name; }
    void setName(std::string name) { this->name = name; }
    Channel(std::string name) : name(name)
    {
        inviteOnly = false;
        maxClients = -1;
        clients = 1;
        topic.clear();
        pass.clear();
    }
    void setOperatorNick(std::string nick) { operatorNick = nick; }
    std::string getPass() { return pass; }
    void setPass(std::string pass) { this->pass = pass; }
    std::string getOperatorNick() { return operatorNick; }
    bool getInviteOnly() { return inviteOnly; }
    int getMaxClients() { return maxClients; }
    int getClients() { return clients; }
    void addClient() { clients++; }
    void removeClient() { clients--; }
    std::string getTopic() { return topic; }
    void sendToEverybody(std::string stringToSend, std::vector<Client*> clients)
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
                if (*ch_it == name)
                {
                    send((*cl_it)->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
                    break;
                }
                ++ch_it;
            }
            ++cl_it;
        }
    }

};

// #include "client.hpp"
#endif