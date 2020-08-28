#include "ScmObject_InternalError.h"

ScmObject_InternalError::ScmObject_InternalError(std::string _message) : ScmObject(ScmObjectType::INTERNAL_ERROR)
{
	m_message = _message;
}

std::string ScmObject_InternalError::getMessage() const
{
	return m_message;
}

bool ScmObject_InternalError::equals(const ScmObject* _other) const
{
	throw new std::exception("Equals() may never be called on an internal error object (ScmObject_InternalError). This means there is an error in code.");
}

ScmObject* ScmObject_InternalError::copy() const
{
	return new ScmObject_InternalError(*this);
}

const std::string ScmObject_InternalError::getOutputString() const
{
	return "Internal error: " + m_message;
}
