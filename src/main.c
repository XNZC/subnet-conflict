#include "ubus.h"

int main(){
    start_ubus_server();
    ubus_cleanup();

    return 0;
}