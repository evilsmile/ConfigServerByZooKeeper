CC=g++
ZK_INCLUDE_PATH=/home/evil//zf-3.4.6/include/zookeeper
ZK_LIB_PATH=/home/evil//zf-3.4.6/lib/
CPPFLAGS=-Wall -g -I${ZK_INCLUDE_PATH}
LDFLAGS=-L${ZK_LIB_PATH} -lzookeeper_mt -lpthread

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
