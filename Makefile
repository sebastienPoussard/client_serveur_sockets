
SRCS =	./client_echo.c \
	./lib.c

#===============================================================================

CC	= gcc
CFLAGS  = -Wall
CFLAGS += -Werror=implicit-function-declaration
all : client

client : $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ && ./$@ 

clean :
	$(RM) output

