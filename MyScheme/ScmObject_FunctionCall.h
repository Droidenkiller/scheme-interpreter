#pragma once
#include "ScmObject.h"
#include "ScmObject_Symbol.h"
#include "ScmObject_Cons.h"
#include <vector>
#include "Environment.h"
#include "ScmObject_FunctionExecution.h"
#include <memory>

class ScmObject_FunctionCall :
	public ScmObject
{
friend class ScmObject_FunctionExecution;

private:
	// TODO: This may be a udf instead of a symbol. Implement it this way.
	std::shared_ptr<ScmObject_Symbol> m_functionSymbol;
	std::vector<std::shared_ptr<ScmObject>> m_functionArgs;

public:
	ScmObject_FunctionCall(std::shared_ptr<ScmObject_Symbol> _functionSymbol, std::vector<std::shared_ptr<ScmObject>> _args);

	std::shared_ptr<ScmObject_FunctionExecution> createFunctionExecution(std::shared_ptr<Environment> _env, std::shared_ptr<Environment> _closureEnv) const;

	std::shared_ptr<const ScmObject_Symbol> getFunctionSymbol() const;

	// TODO: Is it possible to make the entries of the vector const?
	const std::vector<std::shared_ptr<ScmObject>>* getFunctionArgs() const;

	bool equals(const ScmObject* _other) const;

	std::shared_ptr<const ScmObject> copy() const;

	std::string const getOutputString() const;

	const std::string getDisplayString() const;

	std::shared_ptr<ScmObject_Cons> makeCons() const;
};

