#ifndef FOOCAD_PLUSCOVER_H
#define FOOCAD_PLUSCOVER_H

#include <string>

class PlusCover
{
public:
	PlusCover();
	~PlusCover();

	std::wstring m_TempPath;
	std::wstring m_CoverPath;

	std::wstring GetCover(std::wstring FilePath);

private:
	unsigned int GenCover(std::wstring FilePath);
	bool SearchCover(std::wstring FilePath);
};

#endif