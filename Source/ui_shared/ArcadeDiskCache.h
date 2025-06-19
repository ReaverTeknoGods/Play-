#pragma once

#include <unordered_map>
#include <string>
#include <mutex>
#include "filesystem_def.h"

class CArcadeDiskCache
{
public:
	static CArcadeDiskCache& GetInstance();
	
	bool GetCachedDiskId(const fs::path& imagePath, std::string* diskIdPtr);
	void CacheDiskId(const fs::path& imagePath, const std::string& diskId);
	void ClearCache();

private:
	CArcadeDiskCache() = default;
	
	std::unordered_map<std::string, std::string> m_diskIdCache;
	std::mutex m_cacheMutex;
};
