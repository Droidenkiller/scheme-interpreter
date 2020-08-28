#include "ScmObject_Symbol.h"

ScmObject_Symbol::ScmObject_Symbol(std::string _name) : ScmObject(ScmObjectType::SYMBOL)
{
	m_name = _name;
}

const std::string* ScmObject_Symbol::getName() const
{
	return &m_name;
}

bool ScmObject_Symbol::equals(const ScmObject* _other) const
{
	if (_other->getType() != ScmObjectType::SYMBOL)
	{
		return false;
	}
	else
	{
		return m_name.compare(*static_cast<const ScmObject_Symbol*>(_other)->getName()) == 0;
	}
}

ScmObject* ScmObject_Symbol::copy() const
{
	return new ScmObject_Symbol(*this);
}

const std::string ScmObject_Symbol::getOutputString() const
{
	return m_name;
}
