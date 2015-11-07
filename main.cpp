#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <zookeeper.h>
#include <zookeeper_log.h>
#include <pthread.h>
#include <errno.h>

#include <sstream>
#include <iostream>

static std::string g_timeout;
static std::string g_timeout_key = "/config/timeout";
static zhandle_t *g_zkHandler;

void getConfigByPath(zhandle_t* zkHandler, const std::string&  strPath);

void zk_watcher_g(zhandle_t* zh, int type, int state, const char* path,void *watcherCtx)
{
	static std::string EVENT[] = {
		"ZOO_SESSION_EVENT",
		"ZOO_NOTWATCHING_EVENT",
		"ZOO_CREATED_EVENT",
		"ZOO_DELETED_EVENT",
		"ZOO_CHANGED_EVENT",
		"ZOO_CHILD_EVENT"
	};

	std::ostringstream oss;
	oss << "Event [" << EVENT[type+2] << "]"
			<< " of path [" 	<< path << "] happened."
			<< " Connection state: [" << state  << "]"
			<< " Watcher context: [" << (char*)watcherCtx << "]";

	std::cout << oss.str() << std::endl;

	if (strncmp(path, g_timeout_key.c_str(), g_timeout_key.size()) == 0)
	{
		std::cout << "TIMEOUT Config UPDATED!\n";
		getConfigByPath(zh, path);
	}
}

void create(zhandle_t* zkHandler, const std::string strPath, const std::string& strNodeName)
{
	std::cout << "[Create node in sync mode...]\n";

	char path_buffer[64];
	int bufferlen = sizeof(path_buffer);
	int flag = zoo_create(zkHandler, 
			strPath.c_str(), strNodeName.c_str(), strNodeName.size(),
			&ZOO_OPEN_ACL_UNSAFE, 0,
			path_buffer, bufferlen
			);
	if (flag != ZOK)
	{
		fprintf(stderr, "Node '%s' create failed.\n", strNodeName.c_str());
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Node '%s' created OK!\n", path_buffer);
	}

}

void getConfigByPath(zhandle_t* zkHandler, const std::string&  strPath)
{
	std::cout << "[Get data in sync mode...]\n";
	char buffer[64];
	int bufferLen = sizeof(buffer);
	memset(buffer, 0, bufferLen);

	//1 implies that watcher is enabled.
	int flag = zoo_get(zkHandler, 
			strPath.c_str(), 1,
			buffer, &bufferLen, NULL
			);
	if (flag == ZOK)
	{
		g_timeout.assign(buffer, bufferLen);
		printf("global timeout config: %s\n", g_timeout.c_str());
	} 
	else 
	{
		fprintf(stderr, "ERROR get data of node '%s'\n", strPath.c_str());
	}
}	

void exists(zhandle_t* zkHandler, char *path)
{
	int flag = zoo_exists(zkHandler, path, 1, NULL);

	printf("'%s' exists flag: %d\n", path, flag);
}

void getChildren(zhandle_t *zkHandler, char *path)
{
	
}

void* doWatchZK(void *host)
{
	int timeout = 30000;
	char hello[] = "Hello Zookeeper";

	zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);

	g_zkHandler = zookeeper_init((char*)host, zk_watcher_g, timeout, 0, hello, 0);
	if (g_zkHandler == NULL)
	{
		std::cerr << "ERROR: connect to zookeeper server failed!!" << std::endl;
		int ret = EXIT_FAILURE;
		pthread_exit(&ret);
	}

	getConfigByPath(g_zkHandler, g_timeout_key);

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

	zookeeper_close(g_zkHandler);

	return 0;
}
