CC=g++
CPPFLAGS=-Wall -g -I/usr/local/include/zookeeper
LIB=zookeeper_mt
LDFLAGS= -l${LIB} -lpthread

BIN=config_server_by_zk
SRCS=$(shell ls *.cpp)
OBJS=$(subst .cpp,.o,${SRCS})

all:$(BIN)

$(BIN):$(OBJS) 
	$(CC) -o $@ $^ $(CPPFLAGS) $(LDFLAGS)

%.o:%.cpp
	$(CC) -c $(CPPFLAGS) -o $@ $<

clean:
	$(RM) -rf *.o $(BIN)

