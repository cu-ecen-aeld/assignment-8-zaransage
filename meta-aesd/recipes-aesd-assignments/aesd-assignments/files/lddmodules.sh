#!/bin/sh

case "$1" in
  stop)
    /etc/init.d/faulty_init.sh stop
    /etc/init.d/hello_init.sh stop
    /etc/init.d/scull_init.sh stop
    /etc/init.d/aesd_init.sh stop

esac

exit 0