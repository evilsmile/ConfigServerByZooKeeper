#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <sstream>
#include <iostream>

#include "ZKWrapper.h"

ZKWrapper::ZKWrapper(int timeout) : m_nTimeout(timeout)
{

}

ZKWrapper::~ZKWrapper()
{
	if (m_zkHandler)
	{
		zookeeper_close(m_zkHandler);
	}
}

int ZKWrapper::Init(const char *host)
{
	zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);

	m_zkHandler = zookeeper_init((char*)host, zkWatcherCallback, m_nTimeout, 0, this, 0);
	if (m_zkHandler == NULL) 
	{
		std::cerr << "ERROR: connect to zookeeper server failed!!" << std::endl;
		return -1;
	}
	return 0;
}

void zkWatcherCallback(zhandle_t* zh, int type, int state, const char* path,void *watcherCtx)
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

	if (type != ZOO_SESSION_EVENT && type != ZOO_NOTWATCHING_EVENT) 
	{
		ZKWrapper* p_zkWrapper = (ZKWrapper*)watcherCtx;
		std::string strData;
		p_zkWrapper->getDataByPath(path, strData);
		std::cout << "data: " << strData << "...\n";
		p_zkWrapper->m_mapData[path] = strData;

		(p_zkWrapper->m_watcherCallback)();
	}
}

void ZKWrapper::setWatchCallback(WatcherCallback cb)
{
		m_watcherCallback = cb;
}

void ZKWrapper::getData(const std::string& strPath, std::string& strData)
{
	std::map<std::string, std::string>::iterator iter;
	iter = m_mapData.find(strPath);
	if (iter == m_mapData.end())
	{
		getDataByPath(strPath, strData);
	} 
	else
	{
		strData = iter->second;
	}
}

void ZKWrapper::getDataByPath(const std::string& strPath, std::string& strData)
{
	std::cout << "[Get data in sync mode...]\n";
	char buffer[64];
	int bufferLen = sizeof(buffer);
	memset(buffer, 0, bufferLen);

	//1 implies that watcher is enabled.
	int flag = zoo_get(m_zkHandler, 
			strPath.c_str(), 1,
			buffer, &bufferLen, NULL
			);
	if (flag == ZOK)
	{
		strData.assign(buffer, bufferLen);
	} 
	else 
	{
		fprintf(stderr, "ERROR when getting data of node '%s'\n", strPath.c_str());
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

void exists(zhandle_t* zkHandler, char *path)
{
	int flag = zoo_exists(zkHandler, path, 1, NULL);

	printf("'%s' exists flag: %d\n", path, flag);
}

void getChildren(zhandle_t *zkHandler, char *path)
{
	
}
