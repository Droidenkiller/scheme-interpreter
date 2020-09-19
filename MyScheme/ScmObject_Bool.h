#pragma once
#include "ScmObject.h"
#include <memory>

class ScmObject_Bool :
	public ScmObject
{
private:
	bool m_value;

public:
	ScmObject_Bool(bool _value);

	const bool getValue() const;

	bool equals(const ScmObject* _other) const;

	std::shared_ptr<const ScmObject> copy() const;

	const std::string getOutputString() const;

	const std::string getDisplayString() const;
};

