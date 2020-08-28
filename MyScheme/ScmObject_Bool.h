#pragma once
#include "ScmObject.h"
class ScmObject_Bool :
	public ScmObject
{
private:
	bool m_value;

public:
	ScmObject_Bool(bool _value);

	const bool getValue() const;

	bool equals(const ScmObject* _other) const;

	ScmObject* copy() const;

	const std::string getOutputString() const;
};

