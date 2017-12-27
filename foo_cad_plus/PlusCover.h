#ifndef FOOCAD_PLUSCOVER_H
#define FOOCAD_PLUSCOVER_H

#include <string>

class PlusCover
{
public:
	PlusCover();
	~PlusCover();

	unsigned long m_hCryptProv;

	std::wstring m_wstrTempDir;

	std::wstring GetCover(const std::wstring & FilePath);

private:
	bool SearchCacheFile(const std::wstring & CacheName, std::wstring & CoverPath);
	bool GenCover(const std::wstring & FilePath, std::wstring & CoverPath);
	bool SearchCover(const std::wstring & FilePath, std::wstring & CoverPath);

	bool GetCacheName(const std::wstring & FilePath, std::wstring & CacheName);

	void CreateCrypt();
	void ReleaseCrypt();
};

#endif