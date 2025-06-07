#!/bin/sh
# Source Load: https://github.com/cu-ecen-aeld/ldd3/blob/master/misc-modules/module_load
# Source Unload: https://github.com/cu-ecen-aeld/ldd3/blob/master/misc-modules/module_unload

module="faulty"
device="faulty"
mode="664"

# Group: since distributions do it differently, look for wheel or use staff

if grep -q '^staff:' /etc/group; then
    group="staff"
else
    group="wheel"
fi

# Regular command handling

case "$1" in
  start)
    echo "Loading module $module"
    insmod /lib/modules/$(uname -r)/extra/$module.ko || exit 1

    # retrieve major number
    major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
    
    if [ ! -z "$major" ]; then
        rm -f /dev/${device}
        mknod /dev/${device} c $major 0
        chgrp $group /dev/${device}
        chmod $mode  /dev/${device}
    else
        echo "No device found in /proc/devices for $module"
    fi
    ;;
  stop)
    echo "Unloading module $module"
    rmmod $module || exit 1
    rm -f /dev/${device}
    ;;
  *)
    echo "Usage: $0 {start|stop}"
    exit 1
esac

exit 0