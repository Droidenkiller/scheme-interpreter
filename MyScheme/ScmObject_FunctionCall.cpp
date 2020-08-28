#include "ScmObject_FunctionCall.h"
#include "BuiltinFunctions.h"
#include <iostream>

using namespace std;

ScmObject_FunctionCall::ScmObject_FunctionCall(ScmObject_Symbol* _functionSymbol, vector<ScmObject*> _args) : ScmObject(ScmObjectType::FUNCTION_CALL)
{
	m_functionSymbol = _functionSymbol;
	m_functionArgs = _args;
}

const ScmObject_Symbol* ScmObject_FunctionCall::getFunctionSymbol() const
{
	return m_functionSymbol;
}

const std::vector<ScmObject*>* ScmObject_FunctionCall::getFunctionArgs() const
{
	return &m_functionArgs;
}

ScmObject_FunctionExecution* ScmObject_FunctionCall::createFunctionExecution(Environment* _env, Environment* _closureEnv) const
{
	return new ScmObject_FunctionExecution(this, _env, _closureEnv);
}

bool ScmObject_FunctionCall::equals(const ScmObject* _other) const
{
	throw new std::exception("Equals() may never be called on a function call object (ScmObject_FunctionCall). This means there is an error in code.");
}

ScmObject* ScmObject_FunctionCall::copy() const
{
	return new ScmObject_FunctionCall(*this);
}

const std::string ScmObject_FunctionCall::getOutputString() const
{
	std::string output = "(" + m_functionSymbol->getOutputString();

	for (auto arg : m_functionArgs)
	{
		output += " " + arg->getOutputString();
	}

	output += ") Function Call";

	return output;
}