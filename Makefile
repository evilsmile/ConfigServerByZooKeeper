CC=g++
ZK_BASE_DIR=./zk-3.4.6/
ZK_INCLUDE_PATH=${ZK_BASE_DIR}/include/zookeeper
ZK_LIB_PATH=${ZK_BASE_DIR}/lib/
CPPFLAGS=-Wall -g -I${ZK_INCLUDE_PATH}
LIB=zookeeper_mt
LDFLAGS=-L${ZK_LIB_PATH} -l${LIB} -lpthread

BIN=config_server_by_zk
SRCS=$(shell ls *.cpp)
OBJS=$(subst .cpp,.o,${SRCS})

all:$(LIB) $(BIN)

$(LIB):
	@if [ ! -d ${ZK_LIB_PATH} ]; then cd ${ZK_BASE_DIR} && ./configure --prefix=`pwd` && make && make install; fi

$(BIN):$(OBJS) 
	$(CC) -o $@ $^ $(CPPFLAGS) $(LDFLAGS)

%.o:%.cpp
	$(CC) -c $(CPPFLAGS) -o $@ $<

clean:
	$(RM) -rf *.o $(BIN)

clean_lib:
	$(RM) -rf ${ZK_LIB_PATH}
