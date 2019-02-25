#include "../csocket.h"

int main(int argc, char* argv[])
{
    if(argc == 4)
    {
        cSocket server(argv[1],                     // localhost
                       std::stoi(argv[2]),          // порт
                       argv[3]);                    // путь для сохранения (напр, "home/[user]/Server")
        server.listen();
    }
    return 0;
}

