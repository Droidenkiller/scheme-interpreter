#include "ScmObject.h"

ScmObject::ScmObject(ScmObjectType _type)
{
	type = _type;
}
	
const ScmObjectType ScmObject::getType() const
{
	return type;
}