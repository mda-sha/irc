#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>
#include <sys/fcntl.h>
#include <cstring>
#include <csignal>
#include "channel.hpp"
#include "client.hpp"
#include <iostream>
#include <string>
#include <ctime>

#define OPER_PASS "oper_pass"
void sig(int sig);


void removeChannel(std::vector<Channel*> &channels, std::string channelName);
std::string makeStringAfterPrefix(std::vector<std::string> cmd);
int checkExistingNicknames(std::string const &nickname, std::vector<Client*>clients);


class irc
{
private:
    std::vector<Client*> clients;
    std::vector<Channel*> channels;
    std::string g_password;
    std::string oper_password;
    int port;
    int sock;
    std::string remainder;

public:

    void closeAndErase(int i);
    void eraseAll();
    std::vector<std::string> splitString(std::string buf);
    void getRegistered(std::vector<std::string> cmd, int i);
    std::vector<int> makeChannelIndArray(std::string channelName, std::string nick);
    void privmsg(std::vector<std::string> cmd, int i, bool notice);
    void away(std::vector<std::string> cmd, int i);
    void userhost(std::vector<std::string> cmd, int i);
    void oper(std::vector<std::string> cmd, int i);
    bool checkChannelName(std::string name);
    std::string makeUsersInChannelString(std::string channelName, int i);
    void sendMsgAfterJoin(std::vector<std::string> cmd, int i, Channel newChannel);
    void join(std::vector<std::string> cmd, int i);
    void part(std::vector<std::string> cmd, int i);
    void invite(std::vector<std::string> cmd, int i);
    void kick(std::vector<std::string> cmd, int i);
    void topic(std::vector<std::string> cmd, int i);
    void modeT(std::vector<std::string> cmd, int i, std::string const &charFrst, std::vector<Channel*>::iterator it_ch);
    void modeL(std::vector<std::string> cmd, int i, std::string const &charFrst, std::vector<Channel*>::iterator it_ch);
    void modeK(std::vector<std::string> cmd, int i, std::string const &charFrst, std::vector<Channel*>::iterator it_ch);
    void mode(std::vector<std::string> cmd, int i);
    void quit(int i);
    void names(std::vector<std::string> cmd, int i);
    void list(std::vector<std::string> cmd, int i);
    void kill(std::vector<std::string> cmd, int i);
    void wallops(std::vector<std::string> cmd, int i);
    void pong(int i);
    void ping();
    void whichCmd(std::vector<std::string> cmd, int i);
    void prepareBuf(char *buf, int i);
    void work(sockaddr_in addr, std::vector<struct pollfd> fds);
    void launchIrcServer(int port, std::string pass);
};