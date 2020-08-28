#include "ScmObject_Integer.h"
#include <string>
#include <sstream>

using namespace std;

int charToNumber(char _input)
{
	return _input - 48;
}

ScmObject_Integer::ScmObject_Integer(string _intLiteral) : ScmObject(ScmObjectType::INT)
{
	m_value = 0;
	bool isNegative = false;

	if (_intLiteral[0] == '-')
	{
		isNegative = true;
	}

	for (size_t i = isNegative ? 1 : 0; i < _intLiteral.length(); ++i)
	{
		m_value += charToNumber(_intLiteral[i]) * pow(10, _intLiteral.length() - (size_t)1 - i);
	}

	if (isNegative)
	{
		m_value *= -1;
	}
}

ScmObject_Integer::ScmObject_Integer(long long _value) : ScmObject(ScmObjectType::INT)
{
	m_value = _value;
}

const long long ScmObject_Integer::getValue() const
{
	return m_value;
}

bool ScmObject_Integer::equals(const ScmObject* _other) const
{
	if (_other->getType() != ScmObjectType::INT)
	{
		return false;
	}
	else
	{
		return m_value == static_cast<const ScmObject_Integer*>(_other)->getValue();
	}
}

ScmObject* ScmObject_Integer::copy() const
{
	return new ScmObject_Integer(*this);
}

const std::string ScmObject_Integer::getOutputString() const
{
	std::ostringstream strs;
	strs << m_value;
	return strs.str();
}