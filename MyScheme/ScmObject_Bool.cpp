#include "ScmObject_Bool.h"

ScmObject_Bool::ScmObject_Bool(bool _value) : ScmObject(ScmObjectType::BOOL)
{
	m_value = _value;
}

const bool ScmObject_Bool::getValue() const
{
	return m_value;
}

bool ScmObject_Bool::equals(const ScmObject* _other) const
{
	if (_other->getType() != ScmObjectType::BOOL)
	{
		return false;
	}
	else
	{
		return m_value == static_cast<const ScmObject_Bool*>(_other)->getValue();
	}
}

std::shared_ptr<const ScmObject> ScmObject_Bool::copy() const
{
	return std::make_shared<const ScmObject_Bool>(*this);
}

const std::string ScmObject_Bool::getOutputString() const
{
	return m_value ? "#t" : "#f";
}

const std::string ScmObject_Bool::getDisplayString() const
{
	return getOutputString();
}