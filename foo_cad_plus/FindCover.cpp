#include "FindCover.h"

#include <fstream>
#include <vector>

using namespace std;

bool FindCover::GetEmbedded(const TagLib::FileRef & fr, std::wstring & target)
{
	if (CCover::GetEmbedded(fr, target))
	{
		char buffer[10];

		fstream fs(target, ios::in | ios::binary);

		fs.read(buffer, 10);

		fs.close();

		wstring OldPath = target;

		if (!memcmp(&buffer[0], "\xff\xd8", 2))
		{
			target += L".jpg";
		}
		else if (!memcmp(&buffer[1], "PNG", 3))
		{
			target += L".png";
		}
		else if (!memcmp(&buffer[0], "BM", 2))
		{
			target += L".bmp";
		}
		else if (!memcmp(&buffer[0], "GIF", 3))
		{
			target += L".gif";
		}
		else
			return true;

		_wrename(OldPath.c_str(), target.c_str());
		return true;
	}
	return false;
}
