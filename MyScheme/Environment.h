#pragma once
#include "ScmObject.h"
#include "ScmObject_Symbol.h"
#include <string>
#include <unordered_map>

class Environment
{
private:
	std::unordered_map<std::string, const ScmObject*> m_environmentMap;
	Environment* m_parentEnvironment = nullptr;

public:
	Environment(Environment* _parentEnv = nullptr);

	void setParent(Environment* _parentEnv);

	const ScmObject* getSymbol(const ScmObject_Symbol* _symbol) const;

	void addSymbol(const ScmObject_Symbol* _symbol, const ScmObject* _object);
};