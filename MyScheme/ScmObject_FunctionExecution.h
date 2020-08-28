#pragma once
#include <vector>
#include "Environment.h"
#include "ScmObject.h"

class ScmObject_FunctionCall;

class ScmObject_FunctionExecution :
	public ScmObject
{
private:
	bool m_argsReplacedBefore = false;

public:
	ScmObject_FunctionCall* functionCall;
	Environment* environment;
	Environment* closureEnvironment;

	ScmObject_FunctionExecution(const ScmObject_FunctionCall* m_functionCall, Environment* _env, Environment* _closureEnv);

	~ScmObject_FunctionExecution();

	const std::vector<ScmObject*>* getFunctionArgs();

	/** Replaces the arguments that come before the argument at index _argAtCancel and sets the argument
		at _argAtCancel to nullptr to mark that it was cancelled there.
	*/
	void replaceArgs(int _argAtCancel, ScmObject* _newArg);

	void replaceArgAt(int _argAt, ScmObject* _newArg, bool _deleteOldArg = false);

	bool equals(const ScmObject* _other) const;

	ScmObject* copy() const;

	const std::string getOutputString() const;
};

