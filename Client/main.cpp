#include <iostream>
#include "../csocket.h"

int main(int argc, char* argv[])
{
    if(argc == 4)
    {
        cSocket client(argv[1],                     // localhost
                       std::stoi(argv[2]),          // порт
                       argv[3],                     // файл для персылки (напр, "home/[user]/test.txt")
                       false);
        client.sendout();
    }

    return 0;
}

