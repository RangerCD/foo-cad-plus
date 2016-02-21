#include "PlusCover.h"
#include "Cover.h"

using namespace std;

PlusCover::PlusCover()
{
	WCHAR buffer[MAX_PATH];
	GetTempPath(MAX_PATH, buffer);
	GetTempFileName(buffer, L"jpg", 0, buffer);
	m_TempPath = buffer;
	m_TempPath = m_TempPath.substr(0, m_TempPath.rfind(L'.') + 1) + L"jpg";
	MoveFile(buffer, m_TempPath.c_str());
}

PlusCover::~PlusCover()
{
	DeleteFile(m_TempPath.c_str());
}

wstring PlusCover::GetCover(wstring FilePath)
{
	if (!GenCover(FilePath))
		if (!SearchCover(FilePath))
			return wstring();
	return m_TempPath;
}

unsigned int PlusCover::GenCover(wstring FilePath)
{
	TagLib::FileRef fr(FilePath.c_str(), false);
	if (!fr.isNull() && CCover::GetEmbedded(fr, m_TempPath))
	{
		return m_TempPath.length();
	}
	else
	{
		return 0;
	}
	return 0;
}

bool PlusCover::SearchCover(wstring FilePath)
{
	wstring folder = FilePath.substr(0, FilePath.rfind(L"\\") + 1);
	return CCover::GetLocal(L"cover", folder, m_TempPath);
}
