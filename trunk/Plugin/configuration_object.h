#ifndef CONF_OBJECT_H
#define CONF_OBJECT_H

#include "wx/xml/xml.h"
#include "smart_ptr.h"


class wxXmlNode;

/**
 * Interface of configuration objects
 */
class ConfObject {
public:
	ConfObject(){};
	virtual ~ConfObject(){}

	virtual wxXmlNode *ToXml() const = 0;
};

typedef SmartPtr<ConfObject> ConfObjectPtr;
#endif // CONFIGURATION_OBJECT_H
