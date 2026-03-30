#ifndef DUPLICATE_HANDLER_H
#define DUPLICATE_HANDLER_H

#include <ifaddrs.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct address {
    char* name;
    uint8_t prefix_len;
    uint32_t index;
    uint32_t ipaddr;
    struct address* next;
};

struct address* update_list(bool deletion, uint32_t ipaddr, uint32_t index, char* name, uint8_t prefix_len);
void free_addresses();

#endif