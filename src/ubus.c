#include "ubus.h"
#include "netlink.h"
#include "duplicate_handler.h"

#include <stdio.h>
#include <libubus.h>


static void subscribe_cb(struct ubus_context *ctx, struct ubus_object *obj);
static void timer_cb(struct uloop_timeout *t);
static void server_main();

int start_ubus_server();
void ubus_cleanup();

static struct ubus_method methods[] = {};

static struct ubus_object_type obj_type =
    UBUS_OBJECT_TYPE("subnet.monitor", methods);

static struct ubus_object object = {
    .name = "subnet.monitor",
    .type = &obj_type,
    .subscribe_cb = subscribe_cb,
};

static struct ubus_context* ctx = NULL;
static struct blob_buf b;

/* --- */

static void subscribe_cb(struct ubus_context *ctx, struct ubus_object *obj)
{
    fprintf(stderr, "Subscribers active: %d\n", obj->has_subscribers);
}

static void interval_cb(struct uloop_interval *i) {
    
    struct address* dupl = monitor();
    if (dupl == 1) {
        blob_buf_init(&b, 0);
        blobmsg_add_string(&b, "msg", "Duplicates detected");
        blobmsg_add_string(&b, "interface", dupl->name);
        blobmsg_add_u32(&b, "ip", dupl->ipaddr);

        ubus_notify(ctx,  &object, "", b.head, -1);

        blob_buf_free(&b);
    }
}

static void server_main(){
    int ret;

    ret = ubus_register_subscriber(ctx, &object);
    if (ret) {
        fprintf(stderr, "Failed to add object: %s\n", ubus_strerror(ret));
        return;
    }

    struct uloop_interval interval;
    interval.cb = interval_cb;
    uloop_interval_set(&interval, 10);

    ret = init_netlink_monitor();
    if (ret == -1) {
        return;
    }

    uloop_run();
}

int start_ubus_server() {
    uloop_init();
    signal(SIGPIPE, SIG_IGN);
    
    ctx = ubus_connect(NULL);
    if (ctx == NULL) {
        return -1;
    }

    ubus_add_uloop(ctx);
    server_main();

    return 0;
}

void ubus_cleanup() {
    if (ctx != NULL) {
        ubus_free(ctx);
        uloop_done();
        netlink_cleanup();
    }
}