CC = g++
INCLUDE_PATH = /usr/local/zeromq/include
LIB_PATH = /usr/local/zeromq/lib/
LIBS = -lpthread
LIBS += -lzmq
CFLAGS = -g
TARGET = zmq_server

$(TARGET): *.o
	$(CC) -L$(LIB_PATH) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -c $^
	
clean:
	rm -f *.o $(TARGET)
