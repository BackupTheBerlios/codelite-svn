#ifndef BUILDER_WINDOW_H
#define BUILDER_WINDOW_H

#include "wx/string.h"

/**
 * \ingroup Plugin
 * An interface class to the actual wxWindow which exists in the application
 *
 * \date 05-12-2007
 *
 * \author Eran
 *
 */
class IBuilderWindow {
public:
	IBuilderWindow(){};
	virtual ~IBuilderWindow();

	virtual void AppendLine(const wxString &line) = 0;
};

#endif // BUILDER_WINDOW_H