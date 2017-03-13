EXECUTABLE := matchClient

LIBS :=  
INCS :=  
LIBPATHS := 

USER_MARCOS := _POSIX_THREADS _LINUX_OS_ _FILE_OFFSET_BITS=64
CFLAGS = -Wall -static --no-strict-aliasing -g
CC = gcc -std=gnu99

SOURCE := $(wildcard *.c) 
OBJS := $(patsubst %.c,%.o,$(SOURCE))

%.o:%.c
	$(CC) $(CFLAGS) $(addprefix -D,$(USER_MARCOS)) $(addprefix -I,$(INCS)) -c $< -o $@

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(addprefix -L,$(LIBPATHS))  -o $(EXECUTABLE) $(OBJS) $(addprefix -l,$(LIBS))

clean :
	rm -rf *.d *.o *.lo $(EXECUTABLE)
# DO NOT DELETE
