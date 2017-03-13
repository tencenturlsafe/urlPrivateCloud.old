EXECUTABLE_ONLINE_UPDATE := online_update
EXECUTABLE_LOCAL_UPDATE := local_update
EXECUTABLE_MATCH  := match_url

LIBS := url_cloud64 curl crypto m rt pthread 
INCS := ../include 
LIBPATHS := ../lib

USER_MARCOS := _POSIX_THREADS _LINUX_OS_ _FILE_OFFSET_BITS=64
CFLAGS = -Wall -static --no-strict-aliasing -g 
CC = gcc -std=gnu99

ONLINE_SOURCE := online_update.c
LOCAL_SOURCE := local_update.c 
MATCH_SOURCE := url_match.c

ONLINE_OBJS := $(patsubst %.c,%.o,$(ONLINE_SOURCE))
LOCAL_OBJS := $(patsubst %.c,%.o,$(LOCAL_SOURCE))
MATCH_OBJS := $(patsubst %.c,%.o,$(MATCH_SOURCE))

%.o:%.c
	$(CC) $(CFLAGS) $(addprefix -D,$(USER_MARCOS)) $(addprefix -I,$(INCS)) -c $< -o $@

all: $(EXECUTABLE_ONLINE_UPDATE) $(EXECUTABLE_LOCAL_UPDATE) $(EXECUTABLE_MATCH)

$(EXECUTABLE_ONLINE_UPDATE): $(ONLINE_OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS))  -o $(EXECUTABLE_ONLINE_UPDATE) $(ONLINE_OBJS) $(addprefix -l,$(LIBS))

$(EXECUTABLE_LOCAL_UPDATE): $(LOCAL_OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS))  -o $(EXECUTABLE_LOCAL_UPDATE) $(LOCAL_OBJS) $(addprefix -l,$(LIBS))

$(EXECUTABLE_MATCH): $(MATCH_OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS))  -o $(EXECUTABLE_MATCH) $(MATCH_OBJS) $(addprefix -l,$(LIBS))

clean :
	rm -rf *.d *.o *.lo $(EXECUTABLE_ONLINE_UPDATE) $(EXECUTABLE_LOCAL_UPDATE) $(EXECUTABLE_MATCH)
# DO NOT DELETE
