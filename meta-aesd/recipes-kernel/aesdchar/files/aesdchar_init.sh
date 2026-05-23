#!/bin/sh              
# Expand the load and unload scripts as a case file.
 

  module=aesdchar                                                                                                                                                                                         
  device=aesdchar                                                                                                                                                                                         
  mode="664"                                                                                                                                                                                              
                                                                                                                                                                                                          
  if grep -q '^staff:' /etc/group; then
      group="staff"
  else
      group="wheel"
  fi

  case "$1" in
    start)
      insmod /lib/modules/$(uname -r)/extra/$module.ko || exit 1
      major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)                                                                                                                                           
      rm -f /dev/${device}
      mknod /dev/${device} c $major 0                                                                                                                                                                     
      chgrp $group /dev/${device}                                                                                                                                                                         
      chmod $mode /dev/${device}
      ;;                                                                                                                                                                                                  
    stop)         
      rmmod $module || exit 1
      rm -f /dev/${device}
      ;;
    *)
      echo "Usage: $0 {start|stop}"                                                                                                                                                                       
      exit 1
  esac                                                                                                                                                                                                    
                  
  exit 0