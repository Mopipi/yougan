#include <iostream>

#include "loginmodule.h"

using namespace std;


int main(int argc, char *argv[]) {

    Server server;
    server.addModel(NET_MODULE, new NetModule);
    server.addModel(LOGIN_MODULE, new LoginModule);
  
    server.run();
    return 0;
}