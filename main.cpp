#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <pthread.h>
#include <errno.h>

#include <sstream>
#include <iostream>

#include "ZKWrapper.h"

// global config of timeout
static int g_timeout;
static std::string g_timeout_key = "/config/timeout";
static ZKWrapper g_zkWrapper;

void watchCallback()
{
	std::string strData;
	g_zkWrapper.getData(g_timeout_key, strData);

	g_timeout = atoi(strData.c_str());

	std::cout << "Updated timeout value: [" << g_timeout << "]" << std::endl;
}

void* doWatchZK(void *host)
{
	if (g_zkWrapper.Init((char*)host))
	{
		int ret = EXIT_FAILURE;
		pthread_exit(&ret);
	}
	g_zkWrapper.setWatchCallback(watchCallback);

	std::string strData;
	g_zkWrapper.getData(g_timeout_key, strData);

	while(true){}

	return NULL;
}

int InitWatcherZookeeperThread(const char* host)
{
	pthread_t watchZKThreadID;

	if (pthread_create(&watchZKThreadID, NULL, doWatchZK, (void*)host))
	{
		perror("create watcher zk thread failed");
		return -1;
	}

	pthread_join(watchZKThreadID, NULL);

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: host_ip:port\n");
		exit(EXIT_FAILURE);
	}
	const char* host = argv[1];

	if (InitWatcherZookeeperThread(host))
	{
		exit(EXIT_FAILURE);
	}

	return 0;
}
