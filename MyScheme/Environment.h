#pragma once
#include "ScmObject.h"
#include "ScmObject_Symbol.h"
#include <string>
#include <unordered_map>
#include <memory>

class Environment
{
private:
	std::unordered_map<std::string, std::shared_ptr<const ScmObject>> m_environmentMap;
	std::shared_ptr<Environment> m_parentEnvironment = nullptr;

public:
	Environment(std::shared_ptr<Environment> _parentEnv = nullptr);

	void setParent(std::shared_ptr<Environment> _parentEnv);

	std::shared_ptr<const ScmObject> getSymbol(const std::shared_ptr<const ScmObject_Symbol> _symbol) const;

	void addSymbol(const std::shared_ptr<const ScmObject_Symbol> _symbol, const std::shared_ptr<const ScmObject> _object);

	bool setSymbol(const std::shared_ptr<const ScmObject_Symbol> _symbol, std::shared_ptr<const ScmObject> _object);
};