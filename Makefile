################################################################################
### CUSTOMIZE BELOW HERE #######################################################

CC=gcc # define the compiler to use
TARGET=matto # define the name of the executable
SOURCES=main.c board.c input.c protocol.c search.c util.c
CFLAGS=-O3
LFLAGS=

################################################################################
### DO NOT EDIT THE FOLLOWING LINES ############################################

# define list of objects
OBJSC=$(SOURCES:.c=.o)
OBJS=$(OBJSC:.cpp=.o)

# the target is obtained linking all .o files
all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(TARGET)

purge: clean
	rm -f $(TARGET)

clean:
	rm -f *.o

################################################################################
################################################################################
