#include "ScmObject_FunctionExecution.h"
#include "ScmObject_FunctionCall.h"

ScmObject_FunctionExecution::ScmObject_FunctionExecution(const ScmObject_FunctionCall* _functionCall, std::shared_ptr<Environment> _env, std::shared_ptr<Environment> _closureEnv) : ScmObject(ScmObjectType::FUNCTION_EXECUTION)
{
	functionCall = new ScmObject_FunctionCall(*_functionCall);

	environment = _env;
	closureEnvironment = _closureEnv;
}

ScmObject_FunctionExecution::~ScmObject_FunctionExecution()
{
	delete functionCall;
}

const std::vector<std::shared_ptr<ScmObject>>* ScmObject_FunctionExecution::getFunctionArgs()
{
	return &functionCall->m_functionArgs;
}

void ScmObject_FunctionExecution::replaceArgs(int _argAtCancel, std::shared_ptr<ScmObject> _newArg)
{
	for (int i = 0; i < _argAtCancel; ++i)
	{
		functionCall->m_functionArgs.pop_back();
	}

	// Mark that we have replaced the arguments up to here. This function will be called when execution happens
	// and this argument will have been pushed to the stack at this point.
	functionCall->m_functionArgs[functionCall->m_functionArgs.size() - 1] = nullptr;
	functionCall->m_functionArgs.push_back(_newArg);
}

void ScmObject_FunctionExecution::replaceArgAt(int _argAt, std::shared_ptr<ScmObject> _newArg, bool _deleteOld)
{
	functionCall->m_functionArgs[_argAt] = _newArg;
}

void ScmObject_FunctionExecution::removeArgsBefore(int _index, std::shared_ptr<ScmObject> _newArg)
{
	for (int i = 0; i < _index; ++i)
	{
		functionCall->m_functionArgs.erase(functionCall->m_functionArgs.begin());
	}

	functionCall->m_functionArgs[0] = _newArg;
}

bool ScmObject_FunctionExecution::equals(const ScmObject* _other) const
{
	throw new std::exception("Equals() may never be called on a function execution object (ScmObject_FunctionExecution). This means there is an error in code.");
}

std::shared_ptr<const ScmObject> ScmObject_FunctionExecution::copy() const
{
	return std::make_shared<ScmObject_FunctionExecution>(*this);
}

const std::string ScmObject_FunctionExecution::getOutputString() const
{
	return "This should never be printed. If it is, there is a problem with function execution (ScmObject_FunctionExecution).";
}

const std::string ScmObject_FunctionExecution::getDisplayString() const
{
	return getOutputString();
}
