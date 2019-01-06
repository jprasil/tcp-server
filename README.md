# tcp-server
Tcp server runs as a daemon and responds to commands - mem/n and cpu/n

Compilation:
Download the project from the git into an arbitrary directory and then do "make" for a compilation. Makefile is placed in the /prj directory.

1) Compilation in debug mode - In order to print debug messages there is no redirect standard I/O in debug mode.
                        If you want to compile the project in debug mode, then you have to execute "make" with BUILD_MODE
                        parameter.
                        For example:  make BUILD_MODE=debug
                        
2) Compilation in run mode (default) - There is redirected standard I/O to the /dev/null (no messages are printed). 
                        If you want to compile the project in run mode, you can execute "make" without BUILD_MODE parameter.
                        For example:  make
                                  or  make BUILD_MODE=run
                         
