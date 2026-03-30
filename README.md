# subnet-conflict
This project provides a way to detect subnet/IP conflicts on a router using [netlink](https://www.man7.org/linux/man-pages/man7/netlink.7.html)

## Features
  + Detects subnet/IP conflicts
  + Creates an ubus service for broadcasting detected conflicts to subscribed daemons/programs

## Prerequisites
  + Libraries `libubus`, `libubox`
