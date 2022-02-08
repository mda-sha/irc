


class Channel
{
private:
    std::string name;
    bool inviteOnly;
    std::string pass;//////????
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
    }
    void setOperatorNick(std::string nick) { operatorNick = nick; }
    std::string getOperatorNick() { return operatorNick; }
    bool getInviteOnly() { return inviteOnly; }
    int getMaxClients() { return maxClients; }
    int getClients() { return clients; }
    std::string getTopic() { return topic; }

};