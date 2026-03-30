#include "duplicate_handler.h"
/* --- */


static inline struct address* compare_addresses(uint32_t ipaddr, uint32_t index, char* name, uint8_t prefix_len);
static inline struct address* insert_address(uint32_t index, uint32_t ipaddr, char* name, uint8_t prefix_len);
static inline void remove_address(uint32_t index);

struct address* update_list(bool deletion, uint32_t ipaddr, uint32_t index, char* name, uint8_t prefix_len);
void free_addresses();


struct address* first;
struct address* last;

/* --- */

static inline void remove_address(uint32_t index) {
    struct address* previous = NULL;
    
    for (struct address* tmp = first; tmp != NULL; tmp = tmp->next) {

        if (tmp->index != index) {
            previous = tmp;
            continue;
        }

        if (previous != NULL && tmp->next != NULL) {
            previous->next = tmp->next;
            free(tmp);
            return;
        }

        if (previous != NULL) {
            previous->next = NULL;
        }

        if (tmp == first) {
            first = first->next;
        }
        free(tmp->name);
        free(tmp);
        return;
    }
}

static inline struct address* insert_address(uint32_t index, uint32_t ipaddr, char* name, uint8_t prefix_len) {

    fprintf(stdout, "Inserting address:\nindex - %u\nip - %u\n", index, ipaddr);
    struct address* address = (struct address*) malloc(sizeof(struct address));
    if (address == NULL) {
        fprintf(stderr, "Malloc failed\n");
        return;
    }

    address->index = index;
    address->ipaddr = ipaddr;
    address->name = name;
    address->prefix_len = prefix_len;

    address->next = NULL;

    if (first == NULL) {
        first = address;
        last = address;
        return;
    }

    last->next = address;
    last = address;

    return address;
}

static inline uint32_t get_subnet_mask(uint8_t prefix_len){
    return 0xFFFFFFFFU & -(1U << (32 - prefix_len));
}

static inline int compare_subnets(struct address* addr, struct address* addr2){
    uint32_t mask = (addr->prefix_len >= addr2->prefix_len) ? get_subnet_mask(addr->prefix_len) : get_subnet_mask(addr2->prefix_len);

    if ((mask & addr->ipaddr) == (mask & addr2->ipaddr)) {
        return 0;
    }

    return -1;
}

static inline struct address* compare_addresses(uint32_t ipaddr, uint32_t index, char* name, uint8_t prefix_len) {
    struct address* addr = insert_address(index, ipaddr, name, prefix_len); 

    int ret;
    for (struct address* tmp = first; tmp != NULL; tmp = tmp->next) {
        if (ret = compare_subnets(addr, tmp) == 0) {
            return addr;
        }
    }

       
    return NULL;
}

struct address* update_list(bool deletion, uint32_t ipaddr, uint32_t index, char* name, uint8_t prefix_len){
    if (deletion) {
        fprintf(stdout, "Deleting address:\nindex - %u\nip - %u\n", index, ipaddr);
        remove_address(index);
        return NULL;
    }

    for (struct address* tmp = first; tmp != NULL; tmp = tmp->next) {
        if (index == tmp->index && ipaddr != tmp->ipaddr) {
            tmp->ipaddr = ipaddr;
        }
    }

    return compare_addresses(ipaddr, index, name, prefix_len);
}

void free_addresses() {
    struct address* tmp = first;
    for (; tmp != NULL; ) {
        tmp = first->next;
        free(first->name);
        free(first);
        first = tmp;
    }
}