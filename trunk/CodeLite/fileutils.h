#ifndef FILEUTILS_H
#define FILEUTILS_H
#include "wx/filename.h"

class FileUtils {
public:
	static bool ReadFileUTF8(const wxFileName &fn, wxString &data);
};
#endif //FILEUTILS_H
