CC = gcc
CXXFLAGS = -ggdb3 -Wall
TARGET = ftp_breakpoint
all : $(TARGET)

$(TARGET) : ftp_breakpoint.o
	$(CC) $(INCLUDE) ftp_breakpoint.o -o $(TARGET) $(LIB)
ftp_breakpoint.o : ftp_breakpoint.c
	$(CC) $(CXXFLAGS) ftp_breakpoint.c -c

clean :
	rm -f *.o
	rm -f $(TARGET)