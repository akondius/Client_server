#include "csocket.h"

#include <fstream>

cSocket::~cSocket()
{
}

cSocket::cSocket(const std::string &ip,
                 const int port,
                 const std::string &path,
                 const bool fServ)
{
    this->path = path;
    fOpen = false;
    sock=socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cout << "Socket is not open" << std::endl;
        return;
    }
    int length = sizeof(addr);
    if(fServ)
    {
        bzero(&addr, length);
        addr.sin_family=AF_INET;
        addr.sin_addr.s_addr=INADDR_ANY;
        addr.sin_port=htons(port);
        if (bind(sock,
                 reinterpret_cast<struct sockaddr *>(&addr),
                 length)<0)
        {

            std::cout << "Error binding socket" << std::endl;
            return;
        }
    }
    else
    {
        addr.sin_family=AF_INET;
        struct hostent *hp = gethostbyname(ip.c_str());
        if (hp==0)
        {
            std::cout << "Unknown host" << std::endl;
            return;
        }
        bcopy((char *)hp->h_addr,
              reinterpret_cast<char *>(&addr.sin_addr),
              hp->h_length);
        addr.sin_port = htons(port);
    }
    fOpen = true;
    std::cout << (fServ ? "Server" : "Client") << " is open" << std::endl;
}

void cSocket::listen()
{
    if(!fOpen)
        return;
    socklen_t fromlen = sizeof(struct sockaddr_in);
    bool fStart = false;
    int len = 0;
    char buf[1024];
    std::string nameFile;
    std::ofstream fileSave;
    int sz = 0;
    std::string head;
    while (true)
    {
        len = recvfrom(sock, buf, sizeof(buf), 0,
                       reinterpret_cast<struct sockaddr *>(&from),
                       &fromlen);
        if(len > 0)
        {
            std::string tempS = buf;
            if (!fStart)
            {
                if(head == "finish")
                {
                    head.clear();
                    fileSave.close();
                    tempS.substr(0, tempS.find(':'));
                    if(sz != std::stoi(tempS))       // нарушение целостности файла
                        std::cout << "Warning !File no entire." << std::endl;
                    continue;

                }
                head = tempS.substr(0, tempS.find(':'));
                if(head == "stop")      // остановка сервера
                    break;
                if(head != "start")
                    continue;
                tempS.erase(0, tempS.find(':')+1);
                fStart = true;
                nameFile = tempS;
                fileSave.open(path + '/' + nameFile,
                              std::ios::out | std::ios::binary);
                if(!fileSave)
                    fStart = false;
                sz = 0;
                continue;
            }
            if(len == 7)
            {

                head = tempS.substr(0, tempS.find(':'));
                if(head == "finish")
                {
                    fStart = false;
                    continue;
                }
            }
            if(fileSave)
            {
                fileSave.write(buf, len);
                sz += len;
            }
        }
    }
    if(fileSave.is_open())
        fileSave.close();
}

std::string cSocket::shortName() const
{
    std::string tempS = path;
    while(tempS.find('/') != tempS.npos)
        tempS.erase(0, tempS.find('/')+1);
    return tempS;
}

void cSocket::sendout()
{
    std::string mes = "start:"+shortName();
    if(send(mes) > 0)
    {
        // отправка содержимого файла
        std::ifstream file;
        file.open(path.c_str(), std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            std::cout << "File " << path.c_str() << " don't open" << std::endl;
            return;
        }
        const unsigned int size_buf = 1024;
        char buf[size_buf];
        long int sz = 0;
        file.seekg (0, file.end);
        long int length = file.tellg();      // длина файла
        file.seekg (0, file.beg);
        while(true)
        {            
            long int oldTellg = file.tellg();
            file.read(buf, size_buf);
            mes = buf;
            long int curSz = file.tellg()-oldTellg;
            if(file.tellg() < oldTellg)
            {
                curSz = length-oldTellg;
            }
            sz += curSz;
            mes = mes.substr(0, curSz);
            send(mes);
            if(file.eof())
                break;
        }
        send("finish:");  // контрольный размер
        send(std::to_string(sz)+':');
        file.close();
    }
    send("stop:");      // сигнал на остановку сервера
}

int cSocket::send(const std::string &buf)
{
    if(!fOpen)
        return -1;
    socklen_t length=sizeof(struct sockaddr_in);
    return sendto(sock,
                  buf.c_str(),
                  buf.size(),
                  0,
                  reinterpret_cast<struct sockaddr*>(&addr),
                  length);
}
