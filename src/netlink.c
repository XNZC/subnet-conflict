#include <unistd.h>
#include <string.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <ifaddrs.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "duplicate_handler.h"
#include "netlink.h"

#define MAX_BUFFER_LEN 8192

static inline int parse_message(char* buffer, unsigned int len);
static inline int add_n_bind_socket(struct sockaddr_nl* sa, int size);

int init_netlink_monitor();
struct address* monitor();
void netlink_cleanup();

int sock;


int init_netlink_monitor() {
    struct sockaddr_nl sa;
    memset(&sa, 0, sizeof(sa));
    return add_n_bind_socket(&sa, sizeof(sa));
}

struct address* monitor(){
    char buffer[MAX_BUFFER_LEN];
    struct iovec iov = { buffer, sizeof(buffer) };

    struct sockaddr_nl snl;
    struct msghdr msg = { (void*)&snl, sizeof(snl), &iov, 1, NULL, 0, 0 };

    int status = recvmsg(sock, &msg, MSG_DONTWAIT);
    if (status < 0) {
        return;
    }

    return parse_message(buffer, (unsigned int) status);
}

void netlink_cleanup(){
    close(sock);
    free_addresses();
}

static inline int add_n_bind_socket(struct sockaddr_nl* sa, int size){

    sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock == -1) {
        fprintf(stderr, "Failed to create a socket\n");
        return -1;
    }

    sa->nl_family = AF_NETLINK;
    sa->nl_groups = RTMGRP_IPV4_IFADDR;
    sa->nl_pid = getpid();

    if (bind(sock, (struct sockaddr *) sa, size) < 0){
        fprintf(stderr, "Failed to bind a socket\n");
        return -1;
    }

    return 0;
}

static inline int parse_message(char* buffer, unsigned int len){
    struct nlmsghdr* h;

    struct ifaddrmsg* ifa;
    struct rtattr* attr;
    int attr_len;

    for (h = (struct nlmsghdr*) buffer; NLMSG_OK(h, len); h = NLMSG_NEXT(h, len)) {
        
        bool deletion;
        if (h->nlmsg_type == RTM_NEWADDR) {
            deletion = false;
        }else if (h->nlmsg_type == RTM_DELADDR) {
            deletion = true;
        }else {
            break;
        }

        ifa = (struct ifaddrmsg*) NLMSG_DATA(h);
        attr = IFA_RTA(ifa);
        attr_len = IFA_PAYLOAD(h);

        uint8_t prefix_len = ifa->ifa_prefixlen;
        uint32_t ipaddr;
        char* name;

        for (; RTA_OK(attr, attr_len); attr = RTA_NEXT(attr, attr_len)) {
            
            if (attr->rta_type == IFA_LABEL) {
                name = (char*) malloc(sizeof(char) * 128);
                strncpy(name, (char*) RTA_DATA(attr), 128);
            }

            if (attr->rta_type == IFA_ADDRESS) {
                ipaddr = *((uint32_t*) RTA_DATA(attr));
            }
        }

        return update_list(deletion, ipaddr, ifa->ifa_index, name, prefix_len);
    }

    return 0;
}