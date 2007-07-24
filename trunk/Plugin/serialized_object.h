#ifndef SERIALIZED_OBJECT_H
#define SERIALIZED_OBJECT_H

#include "archive.h"

/**
 * \class SerializedObject
 * \brief an interface to the serialized object, every class who whishes to be 
 * serialized and to be read/write from an xml file, must implement this interface
 * \author Eran
 * \date 07/20/07
 */
class SerializedObject
{
public:
	SerializedObject(){}
	virtual ~SerializedObject(){}
	virtual void Serialize(Archive &arch) = 0;
	virtual void DeSerialize(Archive &arch) = 0;
};

#endif //SERIALIZED_OBJECT_H
