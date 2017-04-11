EXECUTABLE_ONLINE_UPDATE := online_update
EXECUTABLE_SINGLE_MATCH := single_match
EXECUTABLE_MULTI_MATCH  := multi_match

LIBS := url_cloud64 curl crypto m rt pthread 
INCS := ../include 
LIBPATHS := ../lib

USER_MARCOS := _POSIX_THREADS _LINUX_OS_ _FILE_OFFSET_BITS=64
CFLAGS = -Wall -static --no-strict-aliasing -g 
CC = gcc -std=gnu99

ONLINE_SOURCE := online_update.c
SINGLE_SOURCE := single_match.c 
MULTI_SOURCE := multi_match.c

ONLINE_OBJS := $(patsubst %.c,%.o,$(ONLINE_SOURCE))
SINGLE_OBJS := $(patsubst %.c,%.o,$(SINGLE_SOURCE))
MULTI_OBJS := $(patsubst %.c,%.o,$(MULTI_SOURCE))

%.o:%.c
	$(CC) $(CFLAGS) $(addprefix -D,$(USER_MARCOS)) $(addprefix -I,$(INCS)) -c $< -o $@

all: $(EXECUTABLE_ONLINE_UPDATE) $(EXECUTABLE_SINGLE_MATCH) $(EXECUTABLE_MULTI_MATCH)

$(EXECUTABLE_ONLINE_UPDATE): $(ONLINE_OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS))  -o $(EXECUTABLE_ONLINE_UPDATE) $(ONLINE_OBJS) $(addprefix -l,$(LIBS))

$(EXECUTABLE_SINGLE_MATCH): $(SINGLE_OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS))  -o $(EXECUTABLE_SINGLE_MATCH) $(SINGLE_OBJS) $(addprefix -l,$(LIBS))

$(EXECUTABLE_MULTI_MATCH): $(MULTI_OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS))  -o $(EXECUTABLE_MULTI_MATCH) $(MULTI_OBJS) $(addprefix -l,$(LIBS))

clean :
	rm -rf *.d *.o *.lo $(EXECUTABLE_ONLINE_UPDATE) $(EXECUTABLE_SINGLE_MATCH) $(EXECUTABLE_MULTI_MATCH)
# DO NOT DELETE
