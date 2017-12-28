#include "PlusCover.h"
#include "FindCover.h"

#include <sstream>
#include <iomanip>
#include <map>
#include <WinCrypt.h>
#include <DbgHelp.h>
#include <io.h>

using namespace std;

const unsigned int e_uiMaxCacheCount = 128;
const unsigned int e_uiPurgeCacheCount = 1024;

PlusCover::PlusCover()
{
	WCHAR buffer[MAX_PATH];
	GetTempPath(MAX_PATH, buffer);
	m_wstrTempDir = buffer;

	if (m_wstrTempDir.back() != L'\\' && m_wstrTempDir.back() != L'/')
		m_wstrTempDir += L"\\";

	m_wstrTempDir += L"foo_cad_plus\\cover\\";

	string TempDir(m_wstrTempDir.length(), '\0');
	copy(m_wstrTempDir.begin(), m_wstrTempDir.end(), TempDir.begin());

	MakeSureDirectoryPathExists(TempDir.c_str());

	ClearCache();
	CreateCrypt();
}

PlusCover::~PlusCover()
{
	ReleaseCrypt();
}

wstring PlusCover::GetCover(const wstring & FilePath)
{
	wstring CacheName;
	wstring CoverPath;
	if (GetCacheName(FilePath, CacheName))
		if (!SearchCacheFile(CacheName, CoverPath))
			if (!GenCover(FilePath, CoverPath = m_wstrTempDir + CacheName))
				if (!SearchCover(FilePath, CoverPath))
					return wstring();
	return CoverPath;
}

bool PlusCover::SearchCacheFile(const std::wstring & CacheName, std::wstring & CoverPath)
{
	return FindCover::GetLocal(CacheName, m_wstrTempDir, CoverPath);
}

bool PlusCover::GenCover(const wstring & FilePath, std::wstring & CoverPath)
{
	TagLib::FileRef fr(FilePath.c_str(), false);
	if (!fr.isNull() && FindCover::GetEmbedded(fr, CoverPath))
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

bool PlusCover::SearchCover(const wstring & FilePath, std::wstring & CoverPath)
{
	wstring folder = FilePath.substr(0, FilePath.rfind(L"\\") + 1);
	wstring parent_folder = folder + L"..\\";
	return (
		//Current Directory
		FindCover::GetLocal(L"cover", folder, CoverPath) ||
		FindCover::GetLocal(L"cover1", folder, CoverPath) ||
		FindCover::GetLocal(L"front", folder, CoverPath) ||
		FindCover::GetLocal(L"front1", folder, CoverPath)
		
		||
		//Parent Directory
		FindCover::GetLocal(L"cover", parent_folder, CoverPath) ||
		FindCover::GetLocal(L"cover1", parent_folder, CoverPath) ||
		FindCover::GetLocal(L"front", parent_folder, CoverPath) ||
		FindCover::GetLocal(L"front1", parent_folder, CoverPath)
		);
}

bool PlusCover::GetCacheName(const std::wstring & FilePath, std::wstring & CacheName)
{
	HCRYPTHASH hHash;
	if (!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &hHash))
	{
		ReleaseCrypt();
		CreateCrypt();
		return false;
	}

	if (!CryptHashData(hHash, (BYTE *)FilePath.c_str(), (FilePath.size()) * sizeof(wchar_t), 0))
	{
		CryptDestroyHash(hHash);
		ReleaseCrypt();
		CreateCrypt();
		return false;
	}

	DWORD dwSize;
	DWORD dwLen = sizeof(DWORD);
	CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)(&dwSize), &dwLen, 0);

	BYTE* pHash = new BYTE[dwSize];
	dwLen = dwSize;
	CryptGetHashParam(hHash, HP_HASHVAL, pHash, &dwLen, 0);

	wstringstream wss;
	wss << std::hex << std::setw(2) << std::setfill(L'0');

	for (DWORD i = 0; i < dwLen; i++)
		wss << pHash[i];

	CacheName = wss.str();

	delete[] pHash;

	CryptDestroyHash(hHash);

	return true;
}

void PlusCover::ClearCache(bool bClearAll)
{
	map<__time64_t, wstring> FileMap;
	unsigned int Count = 0;
	struct _wfinddata_t ff;

	intptr_t hFF = _wfindfirst((m_wstrTempDir + L"*").c_str(), &ff);
	do
	{
		if (wcscmp(ff.name, L".") && wcscmp(ff.name, L".."))
		{
			if (bClearAll)
			{
				_wremove((m_wstrTempDir + ff.name).c_str());
			}
			else
			{
				//Normally you won't get multiple files with same "time_write", 
				//	which means "FileMap.size()" might be different from "Count" 
				//	after traversal in rare cases.
				//These files still have chance to be removed.
				FileMap[ff.time_write] = ff.name;

				++Count;

				if (Count >= e_uiPurgeCacheCount)
				{
					//Purge, too many cache files, remove them all.
					//Did you forget to close Foobar2000 and let it play all week long?
					ClearCache(true);
					_findclose(hFF);
					return;
				}
			}
		}
	} while (_wfindnext(hFF, &ff) == 0);

	//Get recorded count
	Count = FileMap.size();

	for (map<__time64_t, wstring>::iterator it = FileMap.begin(); Count > e_uiMaxCacheCount && it != FileMap.end(); ++it)
	{
		_wremove((m_wstrTempDir + it->second).c_str());
		--Count;
	}

	_findclose(hFF);
}

void PlusCover::CreateCrypt()
{
	if (!CryptAcquireContext(&m_hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		m_hCryptProv = NULL;
	}
}

void PlusCover::ReleaseCrypt()
{
	if (m_hCryptProv)
		CryptReleaseContext(m_hCryptProv, 0);
}
