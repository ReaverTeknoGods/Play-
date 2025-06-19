#include "ArcadeDiskCache.h"
#include "DiskUtils.h"

CArcadeDiskCache& CArcadeDiskCache::GetInstance()
{
	static CArcadeDiskCache instance;
	return instance;
}

bool CArcadeDiskCache::GetCachedDiskId(const fs::path& imagePath, std::string* diskIdPtr)
{
	std::lock_guard<std::mutex> lock(m_cacheMutex);
	
	auto pathStr = imagePath.string();
	auto it = m_diskIdCache.find(pathStr);
	if (it != m_diskIdCache.end())
	{
		if (diskIdPtr)
		{
			*diskIdPtr = it->second;
		}
		return true;
	}
	
	// Cache miss - try to get disk ID and cache the result
	std::string diskId;
	if (DiskUtils::TryGetDiskId(imagePath, &diskId))
	{
		m_diskIdCache[pathStr] = diskId;
		if (diskIdPtr)
		{
			*diskIdPtr = diskId;
		}
		return true;
	}
	
	return false;
}

void CArcadeDiskCache::CacheDiskId(const fs::path& imagePath, const std::string& diskId)
{
	std::lock_guard<std::mutex> lock(m_cacheMutex);
	m_diskIdCache[imagePath.string()] = diskId;
}

void CArcadeDiskCache::ClearCache()
{
	std::lock_guard<std::mutex> lock(m_cacheMutex);
	m_diskIdCache.clear();
}
