# vim: ft=make

# put common definitions in here
CC		 = g++ -rdynamic
DEBUG    = -O0 -g # Uncomment for debug options -O0 -ggdb -pg
CFLAGS	 = $(DEBUG) -Wall -I$(ROOTDIR)include/ -fPIC
CXXFLAGS = $(CFLAGS)
LDFLAGS	 = -g
