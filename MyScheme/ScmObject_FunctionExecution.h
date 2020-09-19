#pragma once
#include <vector>
#include "Environment.h"
#include "ScmObject.h"
#include <memory>

class ScmObject_FunctionCall;

class ScmObject_FunctionExecution :
	public ScmObject
{
public:
	ScmObject_FunctionCall* functionCall;
	std::shared_ptr<Environment> environment;
	std::shared_ptr<Environment> closureEnvironment;

	ScmObject_FunctionExecution(const ScmObject_FunctionCall* m_functionCall, std::shared_ptr<Environment> _env, std::shared_ptr<Environment> _closureEnv);

	~ScmObject_FunctionExecution();

	const std::vector<std::shared_ptr<ScmObject>>* getFunctionArgs();

	/** Replaces the arguments that come before the argument at index _argAtCancel in reverse and sets the argument
		at _argAtCancel to nullptr to mark that it was cancelled there.
	*/
	void replaceArgs(int _argAtCancel, std::shared_ptr<ScmObject> _newArg);

	void replaceArgAt(int _argAt, std::shared_ptr<ScmObject> _newArg, bool _deleteOldArg = false);

	/** Deletes all arguments before the index given in _index and replaces the argument at the index with the argument
		given in _newArg.
	*/
	void removeArgsBefore(int _index, std::shared_ptr<ScmObject> _newArg);

	bool equals(const ScmObject* _other) const;

	std::shared_ptr<const ScmObject> copy() const;

	const std::string getOutputString() const;

	const std::string getDisplayString() const;
};

