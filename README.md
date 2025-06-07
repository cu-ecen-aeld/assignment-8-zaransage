# yocto-assignments-base
Base repository for AESD Yocto assignments

## Commands I needed:

1. ~/git/assignment-6-zaransage$ mkdir -p poky/meta-poky/conf/templates/default
2. source poky/oe-init-build-env (Even if this failed)
2. cd ./pokey (in case from build fails)
2.  ./scripts/devtool add scull https://github.com/cu-ecen-aeld/assignment-7-zaransage
3. ~/git/assignment-6-zaransage/build$ devtool finish scull meta-aesd

4. I had to clean my setup a few times with the bitbake commands and it messed my qemu up.
  1. bitbake -c clean qemu-helper-native
  2. bitbake qemu-helper-native
  3. These commands fixed it at let my normal process work.




## Notes:
1. For some got forsaken reason this environment is never working correctly out of the box.
  1. 'source poky/oe-init-build-env' dies with an error 'Error: TEMPLATECONF value points to nonexistent directory 'meta-poky/conf/templates/default''
  2. I needed 'mkdir -p poky/meta-poky/conf/templates/default' to make the source command work.

2. './poky/scripts/devtool add https://github.com/cu-ecen-aeld/assignment-7-zaransage scull' failed because the init process fails.
  1. './poky/scripts/devtool reset assignment-7-zaransage' was needed before I passed the full path to load things in the correct dir.