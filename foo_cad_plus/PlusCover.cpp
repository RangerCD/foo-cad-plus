#include "PlusCover.h"
#include "Cover.h"

#include <sstream>
#include <iomanip>
#include <WinCrypt.h>
#include <DbgHelp.h>

using namespace std;

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
			if (!GenCover(FilePath, CoverPath = m_wstrTempDir + CacheName + L".jpg"))
				if (!SearchCover(FilePath, CoverPath))
					return wstring();
	return CoverPath;
}

bool PlusCover::SearchCacheFile(const std::wstring & CacheName, std::wstring & CoverPath)
{
	return CCover::GetLocal(CacheName, m_wstrTempDir, CoverPath);
}

bool PlusCover::GenCover(const wstring & FilePath, const std::wstring & CoverPath)
{
	TagLib::FileRef fr(FilePath.c_str(), false);
	if (!fr.isNull() && CCover::GetEmbedded(fr, CoverPath))
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
		CCover::GetLocal(L"cover", folder, CoverPath) ||
		CCover::GetLocal(L"cover1", folder, CoverPath) ||
		CCover::GetLocal(L"front", folder, CoverPath) ||
		CCover::GetLocal(L"front1", folder, CoverPath)
		
		||
		//Parent Directory
		CCover::GetLocal(L"cover", parent_folder, CoverPath) ||
		CCover::GetLocal(L"cover1", parent_folder, CoverPath) ||
		CCover::GetLocal(L"front", parent_folder, CoverPath) ||
		CCover::GetLocal(L"front1", parent_folder, CoverPath)
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
