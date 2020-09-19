#include "ScmObject_Float.h"
#include <sstream>

using namespace std;

int charToInt(char _input)
{
	return _input - 48;
}

ScmObject_Float::ScmObject_Float(string _floatLiteral) : ScmObject(ScmObjectType::FLOAT)
{
	m_value = 0;
	bool isNegative = _floatLiteral[0] == '-';
	int dotIndex = _floatLiteral.find('.');

	for (int i = isNegative ? 1 : 0; i < _floatLiteral.length(); ++i)
	{
		if (i < dotIndex)
		{
			m_value += charToInt(_floatLiteral[i]) * pow(10, dotIndex - 1 - i);
		}
		else if (i > dotIndex)
		{
			m_value += charToInt(_floatLiteral[i]) / pow(10, i - dotIndex);
		}
	}

	if (isNegative)
	{
		m_value *= -1;
	}
}

ScmObject_Float::ScmObject_Float(double _value) : ScmObject(ScmObjectType::FLOAT)
{
	m_value = _value;
}

const double ScmObject_Float::getValue() const
{
	return m_value;
}

bool ScmObject_Float::equals(const ScmObject* _other) const
{
	if (_other->getType() != ScmObjectType::FLOAT)
	{
		return false;
	}
	else
	{
		return m_value == static_cast<const ScmObject_Float*>(_other)->getValue();
	}
}

std::shared_ptr<const ScmObject> ScmObject_Float::copy() const
{
	return std::make_shared<ScmObject_Float>(*this);
}

const std::string ScmObject_Float::getOutputString() const
{
	std::ostringstream strs;
	strs << m_value;
	return strs.str();
}

const std::string ScmObject_Float::getDisplayString() const
{
	return getOutputString();
}
