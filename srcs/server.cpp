#include "../irc.hpp"

void irc::whichCmd(std::vector<std::string> cmd, int i)
{
    if (cmd.size() == 0)
        return;
    if (!clients[i]->getIsAutorized())
        getRegistered(cmd, i);
    else
    {
        if (cmd[0] == "PASS")
        {
            if (!clients[i]->setPassword(g_password, cmd[1]))
                return;
        }
        if (cmd[0] == "USER")
            clients[i]->setName(cmd);
        if (cmd[0] == "NICK")
            clients[i]->setNick(clients, i, cmd[1]);
        if (cmd[0] == "PRIVMSG")
            privmsg(cmd, i, false);
        if (cmd[0] == "AWAY")
            away(cmd, i);
        if (cmd[0] == "NOTICE")
            privmsg(cmd, i, true);
        if (cmd[0] == "USERHOST")
            userhost(cmd, i);
        if (cmd[0] == "OPER")
            oper(cmd, i);
        if (cmd[0] == "JOIN")
            join(cmd, i);
        if (cmd[0] == "PART")
            part(cmd, i);
        if (cmd[0] == "INVITE")
            invite(cmd, i);
        if (cmd[0] == "KICK")
            kick(cmd, i);
        if (cmd[0] == "TOPIC")
            topic(cmd, i);
        if (cmd[0] == "MODE")
            mode(cmd, i);
        if (cmd[0] == "QUIT")
            quit(i);
        if (cmd[0] == "NAMES")
            names(cmd, i);
        if (cmd[0] == "LIST")
            list(cmd, i);
        if (cmd[0] == "KILL")
            kill(cmd, i);
        if (cmd[0] == "WALLOPS")
            wallops(cmd, i);
        if (cmd[0] == "PONG")
            pong(i);
    }
}
void irc::prepareBuf(char *buf, int i)
{
    std::string tmp = buf;
    if (tmp.size() == 1)
        return;
    unsigned long n;
    std::string newBuf;
    std::vector<std::string> cmd;
    while ((n = tmp.find("\n")) != std::string::npos)
    {
        newBuf = tmp.substr(0, n);
        tmp = tmp.substr(n + 1, tmp.size() - n);
        if (newBuf[newBuf.size() - 1] == '\r')
            newBuf.erase(newBuf.size() - 1);
        cmd = splitString(newBuf);
        whichCmd(cmd, i);           
    }
    if (tmp[tmp.size() - 1] == '\r')
        tmp.erase(tmp.size() - 1);
    cmd = splitString(tmp);
    whichCmd(cmd, i); 
}
void irc::work(sockaddr_in addr, std::vector<struct pollfd> fds)
{
    while (1)
    {
        socklen_t clientSize = sizeof(addr);
        int clientSocket = accept(sock, (struct sockaddr*)&addr, &clientSize);
        if (clientSocket >= 0)
        {
            clients.push_back(new Client(clientSocket));
            struct pollfd b;
            b.fd = clientSocket;
            b.events = POLLIN;
            b.revents = 0;
            fds.push_back(b);
            std::cout << "new clientSocket = " << clientSocket << std::endl;
        }
        int a = poll(fds.data(), fds.size(), 1000);
        if (a > 0)
        {
            for (int i = 0; i < static_cast<int>(fds.size()); i++)
            {
                if (fds[i].revents & POLLIN)
                {
                    char buf[512];
                    memset(buf, 0, 100);
                    int bytesRecv;
                    while ((bytesRecv = recv(fds[i].fd, buf, 100, 0) > 0))
                    {
                        clients[i]->setLastActivity();
                        prepareBuf(buf, i);
                    }
                    fds[i].revents = 0;
                }
            }
        }
        ping();
    }
}
void irc::launchIrcServer(int port, std::string pass)
{
    sockaddr_in addr;
    if (port < 1024 || port > 65535)
    {
        std::cout << "port is reserved" << std::endl;
        return;
    }
    this->port = port;
    this->g_password = pass;
    this->oper_password = OPER_PASS;
    addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        std::cout << "socket opening error" << std::endl;
        return;
    }

    int yes = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        std::cout << "binding error" << std::endl;
        return;
    }
    if (listen(sock, SOMAXCONN) == -1)
    {
        std::cout << "binding error" << std::endl;
        return;
    }
	fcntl(sock, F_SETFL, O_NONBLOCK);  
    std::vector<struct pollfd> fds;
    signal(SIGINT, &sig);
    signal(SIGQUIT, &sig);
    work(addr, fds);
}