#ifndef NETLINK_H
#define NETLINK_H

int init_netlink_monitor();
struct address* monitor();
void netlink_cleanup();

#endif