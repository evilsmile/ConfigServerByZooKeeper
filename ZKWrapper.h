#ifndef __ZK_WRAPPER_H__
#define __ZK_WRAPPER_H__

#include <map>

#include <zookeeper.h>
#include <zookeeper_log.h>

typedef void (*WatcherCallback)();
void zkWatcherCallback(zhandle_t* zh, int type, int state, const char* path,void *watcherCtx);

class ZKWrapper {
friend void zkWatcherCallback(zhandle_t* zh, int type, int state, const char* path,void *watcherCtx);
public:
	ZKWrapper(int timeout = 30000);
	~ZKWrapper();

	int Init(const char* host);
	void getData(const std::string& strPath, std::string& strData);
	void setWatchCallback(WatcherCallback cb);

private:
	zhandle_t *m_zkHandler;
	std::map<std::string, std::string> m_mapData;
	WatcherCallback m_watcherCallback;
	int m_nTimeout;

	void getDataByPath(const std::string& strPath, std::string& strData);
};

#endif
