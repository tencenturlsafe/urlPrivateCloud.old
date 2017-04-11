EXECUTABLE_LOCAL_UPDATE := local_update
EXECUTABLE_SINGLE_MATCH := single_match
EXECUTABLE_MULTI_MATCH  := multi_match

LIBS := url_cloud64 curl crypto m rt pthread 
INCS := ../include 
LIBPATHS := ../lib

USER_MARCOS := _POSIX_THREADS _LINUX_OS_ _FILE_OFFSET_BITS=64
CFLAGS = -Wall -static --no-strict-aliasing -g 
CC = gcc -std=gnu99

LOCAL_SOURCE := local_update.c
SINGLE_SOURCE := single_match.c 
MULTI_SOURCE := multi_match.c

LOCAL_OBJS := $(patsubst %.c,%.o,$(LOCAL_SOURCE))
SINGLE_OBJS := $(patsubst %.c,%.o,$(SINGLE_SOURCE))
MULTI_OBJS := $(patsubst %.c,%.o,$(MULTI_SOURCE))

%.o:%.c
	$(CC) $(CFLAGS) $(addprefix -D,$(USER_MARCOS)) $(addprefix -I,$(INCS)) -c $< -o $@

all: $(EXECUTABLE_LOCAL_UPDATE) $(EXECUTABLE_SINGLE_MATCH) $(EXECUTABLE_MULTI_MATCH)

$(EXECUTABLE_LOCAL_UPDATE): $(LOCAL_OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS))  -o $(EXECUTABLE_LOCAL_UPDATE) $(LOCAL_OBJS) $(addprefix -l,$(LIBS))

$(EXECUTABLE_SINGLE_MATCH): $(SINGLE_OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS))  -o $(EXECUTABLE_SINGLE_MATCH) $(SINGLE_OBJS) $(addprefix -l,$(LIBS))

$(EXECUTABLE_MULTI_MATCH): $(MULTI_OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS))  -o $(EXECUTABLE_MULTI_MATCH) $(MULTI_OBJS) $(addprefix -l,$(LIBS))

clean :
	rm -rf *.d *.o *.lo $(EXECUTABLE_LOCAL_UPDATE) $(EXECUTABLE_SINGLE_MATCH) $(EXECUTABLE_MULTI_MATCH)
# DO NOT DELETE
