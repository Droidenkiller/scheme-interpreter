#pragma once
#include "ScmObject.h"
#include "ScmObject_Symbol.h"
#include <vector>
#include "Environment.h"
#include "ScmObject_FunctionExecution.h"

class ScmObject_FunctionCall :
	public ScmObject
{
friend class ScmObject_FunctionExecution;

private:
	ScmObject_Symbol* m_functionSymbol;
	std::vector<ScmObject*> m_functionArgs;

public:
	ScmObject_FunctionCall(ScmObject_Symbol* _functionSymbol, std::vector<ScmObject*> _args);

	ScmObject_FunctionExecution* createFunctionExecution(Environment* _env, Environment* _closureEnv) const;

	std::string const getOutputString() const;

	const ScmObject_Symbol* getFunctionSymbol() const;

	// TODO: Is it possible to make the entries of the vector const?
	const std::vector<ScmObject*>* getFunctionArgs() const;

	bool equals(const ScmObject* _other) const;

	ScmObject* copy() const;
};

