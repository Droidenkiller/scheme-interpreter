#include "Environment.h"

Environment::Environment(std::shared_ptr<Environment> _parentEnv)
{
	m_parentEnvironment = _parentEnv;
}

void Environment::setParent(std::shared_ptr<Environment> _parentEnv)
{
	m_parentEnvironment = _parentEnv;
}

std::shared_ptr<const ScmObject> Environment::getSymbol(const std::shared_ptr<const ScmObject_Symbol> _symbol) const
{
	std::shared_ptr<const ScmObject> result = nullptr;
	const Environment* curEnv = this;

	while (result == nullptr && curEnv != nullptr)
	{
		auto it = curEnv->m_environmentMap.find(*_symbol->getName());

		if (it == curEnv->m_environmentMap.end())
		{
			curEnv = &*curEnv->m_parentEnvironment;
		}
		else
		{
			result = it->second;
			break;
		}
	}

	return result;
}

void Environment::addSymbol(const std::shared_ptr<const ScmObject_Symbol> _symbol, std::shared_ptr<const ScmObject> _object)
{
	m_environmentMap[*_symbol->getName()] = _object;
}

bool Environment::setSymbol(const std::shared_ptr<const ScmObject_Symbol> _symbol, std::shared_ptr<const ScmObject> _object)
{
	bool found = false;
	Environment* curEnv = this;

	while (found == false && curEnv != nullptr)
	{
		auto it = curEnv->m_environmentMap.find(*_symbol->getName());

		if (it == curEnv->m_environmentMap.end())
		{
			curEnv = &*curEnv->m_parentEnvironment;
		}
		else
		{
			curEnv->m_environmentMap[*_symbol->getName()] = _object;
			found = true;
		}
	}

	return found;
}