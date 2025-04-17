 - setup wsl 
 - install github command line in wsl
 - install libdragon to default /opt/libdragon location
 - login w/ `gh auth login`
 - use `gh repo clone MagnetoMallard/n64-ftr` command in the folder you're wantin
 - edit `setvars.sh` to match your local libdragon, t3d and ares installs
 - run `source setvars.sh`

 - run `make && make ares` to build and run it in ares 
 - run `make && make unfloader` to run it on a connected debug cart 
