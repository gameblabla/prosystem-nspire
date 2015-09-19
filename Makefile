# define regarding OS, which compiler to use
PRGNAME     = prosystem-od
CC          = gcc
LD          = gcc

# add SDL dependencies
SDL_LIB     = 
SDL_INCLUDE = /usr/include/SDL

# change compilation / linking flag options
F_OPTS =
CFLAGS      = -I$(SDL_INCLUDE) -O2 
LDFLAGS     = -lSDLmain -lSDL -lm

# Files to be compiled
SRCDIR    = ./emu/zlib  ./emu ./opendingux
VPATH     = $(SRCDIR)
SRC_C   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJ_C   = $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJS     = $(OBJ_C)

# Rules to make executable
$(PRGNAME)$(EXESUFFIX): $(OBJS)  
	$(LD) $(CFLAGS) -o $(PRGNAME)$(EXESUFFIX) $^ $(LDFLAGS)
	
$(OBJ_C) : %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME)$(EXESUFFIX) *.o
