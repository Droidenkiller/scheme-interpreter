#include "Environment.h"
#include <iostream>

Environment::Environment(Environment* _parentEnv)
{
	m_parentEnvironment = _parentEnv;
}

void Environment::setParent(Environment* _parentEnv)
{
	m_parentEnvironment = _parentEnv;
}

const ScmObject* Environment::getSymbol(const ScmObject_Symbol* _symbol) const
{
	const ScmObject* result = nullptr;
	const Environment* curEnv = this;

	while (result == nullptr && curEnv != nullptr)
	{
		auto it = curEnv->m_environmentMap.find(*_symbol->getName());

		if (it == curEnv->m_environmentMap.end())
		{
			curEnv = curEnv->m_parentEnvironment;
		}
		else
		{
			result = it->second;
		}
	}

	return result;
}

void Environment::addSymbol(const ScmObject_Symbol* _symbol, const ScmObject* _object)
{
	m_environmentMap[*_symbol->getName()] = _object;
}