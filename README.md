# tcp-server
Tcp server runs as daemon and respond on two commands - mem/n and cpu/n

Compilation:
Download project from git into an arbitrary directory and then make it. Makefile is placed in the /prj directory.

1) Make in debug mode - In order to print debug messages there isn't redirect standard I/O in debug mode.
                        If you want make project in debug mode you have to execute make with BUILD_MODE parameter.
                        For example:  make BUILD_MODE=debug
                        
2) Make in run mode (default) - There is redirect standard I/O into the /dev/null (no messages are printed). 
                        If you want make project in run mode you can execute make without BUILD_MODE parameter.
                        For example:  make
                                  or  make BUILD_MODE=run
                         
