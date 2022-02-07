


class Channel
{
private:
    std::string name;
    bool inviteOnly;
    std::string pass;//////????
    std::string topic;
    std::string operatorNick;

public:
    std::string getName() {return name; }
    void setName(std::string name) { this->name = name; }
    Channel(std::string name) : name(name) {}
    void setOperatorNick(std::string nick) { operatorNick = nick; }

};