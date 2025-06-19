#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include "Types.h"
#include "filesystem_def.h"
#include "ArcadeDriver.h"

class CNamcoSys246Driver : public CArcadeDriver
{
public:
	void PrepareEnvironment(CPS2VM*, const ARCADE_MACHINE_DEF&) override;
	void Launch(CPS2VM*, const ARCADE_MACHINE_DEF&) override;

private:
	std::vector<uint8> ReadDongleData(const ARCADE_MACHINE_DEF&);
	fs::path LocateImageFile(const ARCADE_MACHINE_DEF&, const std::string&);
	
	// Cache for dongle data to avoid repeated ZIP file access
	static std::unordered_map<std::string, std::vector<uint8>> s_dongleCache;
	static std::mutex s_dongleCacheMutex;
};
