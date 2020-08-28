#include "ScmObject_String.h"

ScmObject_String::ScmObject_String(std::string _value) : ScmObject(ScmObjectType::STRING)
{
	setValue(_value);
}

const std::string* ScmObject_String::getValue() const
{
	return &m_value;
}

void ScmObject_String::setValue(std::string _value)
{
	m_value = _value;
}

bool ScmObject_String::equals(const ScmObject* _other) const
{
	if (_other->getType() != ScmObjectType::STRING)
	{
		return false;
	}
	else
	{
		return m_value.compare(*static_cast<const ScmObject_String*>(_other)->getValue()) == 0;
	}
}

ScmObject* ScmObject_String::copy() const
{
	return new ScmObject_String(*this);
}

const std::string ScmObject_String::getOutputString() const
{
	return m_value;
}