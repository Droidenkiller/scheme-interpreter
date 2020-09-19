#pragma once
#include "ScmObject.h"
#include <string>

class ScmObject_QuoteSymbol :
	public ScmObject
{
private:
	std::string m_text;

public:
	ScmObject_QuoteSymbol(const std::string& _text);

	virtual bool equals(const ScmObject* _other) const;

	virtual std::shared_ptr<const ScmObject> copy() const;

	const std::string getOutputString() const;

	const std::string getDisplayString() const;
};

