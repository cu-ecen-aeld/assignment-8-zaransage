#!/bin/sh

case "$1" in
  stop)
    /etc/rc5.d/S98faulty_init.sh stop
    /etc/rc5.d/S98hello_init.sh stop
    /etc/rc5.d/S98scull_init.sh stop

esac

exit 0