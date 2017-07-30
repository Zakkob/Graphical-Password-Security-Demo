CC = gcc
RM = rm
CFLAGS = -g -I/usr/X11R6/include 
LDFLAGS = -g -L/usr/X11R6/lib -lX11 
TARGET = gra_pass_sec_demo

$(TARGET): $(TARGET).o 
	$(CC) $(CFLAGS) $(TARGET).o -o $@ $(LDFLAGS)

clean:
	$(RM) -f $(TARGET).o $(TARGET)
