#include "ScmObject_QuoteSymbol.h"

ScmObject_QuoteSymbol::ScmObject_QuoteSymbol(const std::string& _text) : ScmObject(ScmObjectType::QUOTE_SYMBOL)
{
	m_text = _text;
}

bool ScmObject_QuoteSymbol::equals(const ScmObject* _other) const
{
	if (_other->getType() != ScmObjectType::QUOTE_SYMBOL)
	{
		return false;
	}
	else
	{
		return m_text.compare(static_cast<const ScmObject_QuoteSymbol*>(_other)->m_text);
	}
}

std::shared_ptr<const ScmObject> ScmObject_QuoteSymbol::copy() const
{
	return std::make_shared<const ScmObject_QuoteSymbol>(*this);
}

const std::string ScmObject_QuoteSymbol::getOutputString() const
{
	return m_text;
}

const std::string ScmObject_QuoteSymbol::getDisplayString() const
{
	return getOutputString();
}
