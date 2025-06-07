# yocto-assignments-base
Base repository for AESD Yocto assignments

## Commands I needed:

1. source poky/oe-init-build-env
2.  ./poky/scripts/devtool add https://github.com/cu-ecen-aeld/assignment-7-zaransage ./build/workspace/sources/scull
3. 


## Notes:
1. For some got forsaken reason this environment is never working correctly.
  1. 'source poky/oe-init-build-env' dies with an error 'Error: TEMPLATECONF value points to nonexistent directory 'meta-poky/conf/templates/default''
  2. Nothing I do, not exports, no file creations, no recloning NOTHING stops this from bitching.
2. './poky/scripts/devtool add https://github.com/cu-ecen-aeld/assignment-7-zaransage scull' failed because the init process fails.
  1. './poky/scripts/devtool reset assignment-7-zaransage' was needed before I passed the full path to load things in the correct dir.