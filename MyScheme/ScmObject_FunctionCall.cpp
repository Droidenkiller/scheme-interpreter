#include "ScmObject_FunctionCall.h"
#include "ScmObject_FunctionDefinition.h"
#include <iostream>

using namespace std;

ScmObject_FunctionCall::ScmObject_FunctionCall(shared_ptr<ScmObject_Symbol> _functionSymbol, vector<shared_ptr<ScmObject>> _args) : ScmObject(ScmObjectType::FUNCTION_CALL)
{
	m_functionSymbol = _functionSymbol;
	m_functionArgs = _args;
}

shared_ptr<const ScmObject_Symbol> ScmObject_FunctionCall::getFunctionSymbol() const
{
	return m_functionSymbol;
}

const std::vector<shared_ptr<ScmObject>>* ScmObject_FunctionCall::getFunctionArgs() const
{
	return &m_functionArgs;
}

std::shared_ptr<ScmObject_FunctionExecution> ScmObject_FunctionCall::createFunctionExecution(shared_ptr<Environment> _env, shared_ptr<Environment> _closureEnv) const
{
	return std::make_shared<ScmObject_FunctionExecution>(this, _env, _closureEnv);
}

bool ScmObject_FunctionCall::equals(const ScmObject* _other) const
{
	throw new std::exception("Equals() may never be called on a function call object (ScmObject_FunctionCall). This means there is an error in code.");
}

std::shared_ptr<const ScmObject> ScmObject_FunctionCall::copy() const
{
	return std::make_shared<ScmObject_FunctionCall>(*this);
}

const std::string ScmObject_FunctionCall::getOutputString() const
{
	std::string output = "(" + m_functionSymbol->getOutputString();

	for (auto arg : m_functionArgs)
	{
		output += " " + arg->getOutputString();
	}

	output += ")";

	return output;
}

const std::string ScmObject_FunctionCall::getDisplayString() const
{
	return getOutputString();
}

std::shared_ptr<ScmObject_Cons> ScmObject_FunctionCall::makeCons() const
{
	std::shared_ptr<ScmObject_Cons> result = nullptr;
	for (auto it = m_functionArgs.rbegin(); it != m_functionArgs.rend(); ++it)
	{
		if ((*it)->getType() == ScmObjectType::FUNCTION_CALL)
		{
			result = std::make_shared<ScmObject_Cons>(static_pointer_cast<ScmObject_FunctionCall>(*it)->makeCons(), result);
		}
		else if ((*it)->getType() == ScmObjectType::FUNCTION_DEFINITION)
		{
			result = std::make_shared<ScmObject_Cons>(static_pointer_cast<ScmObject_FunctionDefinition>(*it)->getFunctionBody(), result);
			result = std::make_shared<ScmObject_Cons>(static_pointer_cast<ScmObject_FunctionDefinition>(*it)->getFunctionArgList(), result);
			result = std::make_shared<ScmObject_Cons>(std::make_shared<ScmObject_Symbol>("lambda"), result);
		}
		else
		{
			result = std::make_shared<ScmObject_Cons>(*it, result);
		}
	}

	result = std::make_shared<ScmObject_Cons>(m_functionSymbol, result);

	return result;
}