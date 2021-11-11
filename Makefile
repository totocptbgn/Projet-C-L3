CC = gcc
CFLAGS = -g -Wall -std=c11 # -Wextra -pedantic
OBJS = projet2019.o main.o
BIN = projet2019

all: clear $(BIN) clean_objs
	
$(BIN): $(OBJS)

clear: # Clear the terminal
	clear
	
clean_objs: # Clean *.o files
	$(RM) $(OBJS)

clean: # Clean binary files
	$(RM) $(BIN) $(OBJS)
