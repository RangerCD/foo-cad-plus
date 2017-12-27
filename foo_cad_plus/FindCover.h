#ifndef FOOCAD_FINDCOVER_H
#define FOOCAD_FINDCOVER_H

#include "Cover.h"

class FindCover
	: public CCover
{
public:
	static bool GetEmbedded(const TagLib::FileRef& fr, std::wstring& target);
};

#endif