#define MEASURE_EXECUTION_TIME
#define STRICT_IF

#include "BuiltInFunctionsTramp.h"
#include "ScmObjectIncludes.h"
#include "Reader.h"
#include <stack>
#include <iostream>
#include <fstream>
#ifdef MEASURE_EXECUTION_TIME
#include <chrono>
#include <iostream>
#endif

using namespace std;

extern shared_ptr<Environment> globalEnvironment;

inline shared_ptr<ScmObject> execTramp(std::shared_ptr<ScmObject_FunctionExecution> _functionCall);

shared_ptr<const ScmObject> lookupInEnv(std::shared_ptr<const ScmObject_FunctionExecution> _exec, const shared_ptr<const ScmObject_Symbol> _symbol)
{
	shared_ptr<const ScmObject> boundSymbol = nullptr;

	if (_exec->closureEnvironment != nullptr)
	{
		boundSymbol = _exec->closureEnvironment->getSymbol(_symbol);
	}
	if (boundSymbol == nullptr)
	{
		boundSymbol = _exec->environment->getSymbol(_symbol);
	}
	if (boundSymbol == nullptr)
	{
		boundSymbol = globalEnvironment->getSymbol(_symbol);
	}

	return boundSymbol;
}

inline void returnError(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _stack, shared_ptr<ScmObject>& _result, std::string _message)
{
	while (_stack.size() > 0)
	{
		_stack.pop();
	}

	_result = make_shared<ScmObject_InternalError>(_message);
}

inline void endExecution(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _stack, shared_ptr<ScmObject>& _result, shared_ptr<ScmObject> _newResult)
{
	_stack.pop();

	_result = _newResult;
}

inline void execBuiltInAdd(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() == 0)
	{
		return returnError(_callStack, _result, "Called built-in add with 0 arguments. Please pass at least one argument.");
	}

	long long intSum = 0;
	double floatSum = 0;
	bool useFloatSum = false;
	int argIndex = 0;

	// Go through in reverse to make replacing arguments faster because we can reduce the argument list from the back.
	for (auto it = args->rbegin(); it != args->rend(); ++it, ++argIndex)
	{
		shared_ptr<ScmObject> arg = *it;

		// We have removed the argument at this place before and it should now be in _result
		if (arg == nullptr)
		{
			arg = _result;
		}

		switch (arg->getType())
		{
		case ScmObjectType::FLOAT:
		{
			if (!useFloatSum)
			{
				useFloatSum = true;
				floatSum = intSum;
			}

			floatSum += static_pointer_cast<ScmObject_Float>(arg)->getValue();
		}
		break;
		case ScmObjectType::INT:
		{
			long long val = static_pointer_cast<ScmObject_Integer>(arg)->getValue();

			if (useFloatSum)
			{
				floatSum += val;
			}
			else
			{
				intSum += val;
			}
		}
		break;
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<ScmObject_Symbol>(arg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in add. Symbol " + *static_pointer_cast<ScmObject_Symbol>(arg)->getName() + " is not defined.");
			}

			switch (obj->getType())
			{
			case ScmObjectType::FLOAT:
			{
				if (!useFloatSum)
				{
					useFloatSum = true;
					floatSum = intSum;
				}

				floatSum += static_pointer_cast<const ScmObject_Float>(obj)->getValue();
			}
			break;
			case ScmObjectType::INT:
			{
				long long val = static_pointer_cast<const ScmObject_Integer>(obj)->getValue();

				if (useFloatSum)
				{
					floatSum += val;
				}
				else
				{
					intSum += val;
				}
			}
			break;
			default:
				return returnError(_callStack, _result, "Error while trying to execute built-in add.");
			}
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(arg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgs(argIndex, useFloatSum ? make_shared<ScmObject_Float>(floatSum) : static_pointer_cast<ScmObject>(make_shared<ScmObject_Integer>(intSum)));
			return;
		}
		break;
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in add.");
		}
	}

	if (useFloatSum)
	{
		return endExecution(_callStack, _result, make_shared<ScmObject_Float>(floatSum));
	}
	else
	{
		return endExecution(_callStack, _result, make_shared<ScmObject_Integer>(intSum));
	}
}

inline void execBuiltInSubtract(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() == 0)
	{
		return returnError(_callStack, _result, "Called built-in subtract with 0 arguments. Please pass at least one argument.");
	}
	if (args->size() == 1)
	{
		auto curArg = (*args)[0];

		if (curArg == nullptr)
		{
			curArg = _result;
		}

		switch (curArg->getType())
		{
		case ScmObjectType::FLOAT:
			return endExecution(_callStack, _result, make_shared<ScmObject_Float>(-static_pointer_cast<ScmObject_Float>(curArg)->getValue()));
		case ScmObjectType::INT:
			return endExecution(_callStack, _result, make_shared<ScmObject_Integer>(-static_pointer_cast<ScmObject_Integer>(curArg)->getValue()));
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<ScmObject_Symbol>(curArg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in subtract. Symbol " + *static_pointer_cast<ScmObject_Symbol>(curArg)->getName() + " is not defined.");
			}

			switch (obj->getType())
			{
			case ScmObjectType::FLOAT:
				return endExecution(_callStack, _result, make_shared<ScmObject_Float>(-static_pointer_cast<const ScmObject_Float>(obj)->getValue()));
			case ScmObjectType::INT:
				return endExecution(_callStack, _result, make_shared<ScmObject_Integer>(-static_pointer_cast<const ScmObject_Integer>(obj)->getValue()));
			default:
				return returnError(_callStack, _result, "Error while trying to execute built-in subtract. Wrong argument type.");
			}
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(curArg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in subtract. Wrong argument type.");
		}
	}

	long long intSum = 0;
	double floatSum = 0;
	bool useFloatSum = false;
	bool lastRun = false;
	int argIndex = 0;

	// Go through in reverse to make replacing arguments faster because we can reduce the argument list from the back.
	for (auto it = args->rbegin(); it != args->rend(); ++it, ++argIndex)
	{
		lastRun = (it + 1) == args->rend();

		shared_ptr<ScmObject> arg = *it;

		// We have removed the argument at this place before and it should now be in _result
		if (arg == nullptr)
		{
			arg = _result;
		}

		switch (arg->getType())
		{
		case ScmObjectType::FLOAT:
		{
			if (!useFloatSum)
			{
				useFloatSum = true;
				floatSum = intSum;
			}

			double val = static_pointer_cast<ScmObject_Float>(arg)->getValue();

			if (lastRun)
			{
				floatSum += val;
			}
			else
			{
				floatSum -= val;
			}
		}
		break;
		case ScmObjectType::INT:
		{
			long long val = static_pointer_cast<ScmObject_Integer>(arg)->getValue();

			if (useFloatSum)
			{
				floatSum -= val;
			}
			else
			{
				if (lastRun)
				{
					intSum += val;
				}
				else
				{
					intSum -= val;
				}
			}
		}
		break;
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<ScmObject_Symbol>(arg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in subtract. Symbol " + *static_pointer_cast<ScmObject_Symbol>(arg)->getName() + " is not defined.");
			}

			switch (obj->getType())
			{
			case ScmObjectType::FLOAT:
			{
				if (!useFloatSum)
				{
					useFloatSum = true;
					floatSum = intSum;
				}

				double val = static_pointer_cast<const ScmObject_Float>(obj)->getValue();

				if (lastRun)
				{
					floatSum += val;
				}
				else
				{
					floatSum -= val;
				}
			}
			break;
			case ScmObjectType::INT:
			{
				long long val = static_pointer_cast<const ScmObject_Integer>(obj)->getValue();

				if (useFloatSum)
				{
					floatSum -= val;
				}
				else
				{
					if (lastRun)
					{
						intSum += val;
					}
					else
					{
						intSum -= val;
					}
				}
			}
			break;
			default:
				return returnError(_callStack, _result, "Error while trying to execute built-in subtract.");
			}
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(arg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgs(argIndex, useFloatSum ? make_shared<ScmObject_Float>(floatSum) : static_pointer_cast<ScmObject>(make_shared<ScmObject_Integer>(intSum)));
			return;
		}
		break;
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in subtract.");
		}
	}

	if (useFloatSum)
	{
		return endExecution(_callStack, _result, make_shared<ScmObject_Float>(floatSum));
	}
	else
	{
		return endExecution(_callStack, _result, make_shared<ScmObject_Integer>(intSum));
	}
}

inline void execBuiltInMultiply(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() == 0)
	{
		return returnError(_callStack, _result, "Called built-in multiply with 0 arguments. Please pass at least one argument.");
	}

	long long intProduct = 1;
	double floatProduct = 1;
	bool useFloatProduct = false;
	int argIndex = 0;

	// Go through in reverse to make replacing arguments faster because we can reduce the argument list from the back.
	for (auto it = args->rbegin(); it != args->rend(); ++it, ++argIndex)
	{
		shared_ptr<ScmObject> arg = *it;

		// We have removed the argument at this place before and it should now be in _result
		if (arg == nullptr)
		{
			arg = _result;
		}

		switch (arg->getType())
		{
		case ScmObjectType::FLOAT:
		{
			if (!useFloatProduct)
			{
				useFloatProduct = true;
				floatProduct = intProduct;
			}

			floatProduct *= static_pointer_cast<ScmObject_Float>(arg)->getValue();
		}
		break;
		case ScmObjectType::INT:
		{
			long long val = static_pointer_cast<ScmObject_Integer>(arg)->getValue();

			if (useFloatProduct)
			{
				floatProduct *= val;
			}
			else
			{
				intProduct *= val;
			}
		}
		break;
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<ScmObject_Symbol>(arg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in multiply. Symbol " + *static_pointer_cast<ScmObject_Symbol>(arg)->getName() + " is not defined.");
			}

			switch (obj->getType())
			{
			case ScmObjectType::FLOAT:
			{
				if (!useFloatProduct)
				{
					useFloatProduct = true;
					floatProduct = intProduct;
				}

				floatProduct *= static_pointer_cast<const ScmObject_Float>(obj)->getValue();
			}
			break;
			case ScmObjectType::INT:
			{
				long long val = static_pointer_cast<const ScmObject_Integer>(obj)->getValue();

				if (useFloatProduct)
				{
					floatProduct *= val;
				}
				else
				{
					intProduct *= val;
				}
			}
			break;
			default:
				return returnError(_callStack, _result, "Error while trying to execute built-in multiply.");
			}
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(arg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgs(argIndex, useFloatProduct ? make_shared<ScmObject_Float>(floatProduct) : static_pointer_cast<ScmObject>(make_shared<ScmObject_Integer>(intProduct)));
			return;
		}
		break;
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in multiply.");
		}
	}

	if (useFloatProduct)
	{
		return endExecution(_callStack, _result, make_shared<ScmObject_Float>(floatProduct));
	}
	else
	{
		return endExecution(_callStack, _result, make_shared<ScmObject_Integer>(intProduct));
	}
}

inline void execBuiltInDivide(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() == 0)
	{
		return returnError(_callStack, _result, "Called built-in divide with 0 arguments. Please pass at least one argument.");
	}
	if (args->size() == 1)
	{
		auto curArg = (*args)[0];

		if (curArg == nullptr)
		{
			curArg = _result;
		}

		switch (curArg->getType())
		{
		case ScmObjectType::FLOAT:
			return endExecution(_callStack, _result, make_shared<ScmObject_Float>(1.0 / static_pointer_cast<ScmObject_Float>(curArg)->getValue()));
		case ScmObjectType::INT:
			return endExecution(_callStack, _result, make_shared<ScmObject_Float>(1.0 / static_pointer_cast<ScmObject_Integer>(curArg)->getValue()));
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<ScmObject_Symbol>(curArg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in divide. Symbol " + *static_pointer_cast<ScmObject_Symbol>(curArg)->getName() + " is not defined.");
			}

			switch (obj->getType())
			{
			case ScmObjectType::FLOAT:
				return endExecution(_callStack, _result, make_shared<ScmObject_Float>(1.0 / static_pointer_cast<const ScmObject_Float>(obj)->getValue()));
			case ScmObjectType::INT:
				return endExecution(_callStack, _result, make_shared<ScmObject_Float>(1.0 / static_pointer_cast<const ScmObject_Integer>(obj)->getValue()));
			default:
				return returnError(_callStack, _result, "Error while trying to execute built-in divide. Wrong argument type.");
			}
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(curArg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in divide. Wrong argument type.");
		}
	}

	double floatProduct = 1;
	bool lastRun = false;
	int argIndex = 0;

	// Go through in reverse to make replacing arguments faster because we can reduce the argument list from the back.
	for (auto it = args->rbegin(); it != args->rend(); ++it, ++argIndex)
	{
		lastRun = (it + 1) == args->rend();

		shared_ptr<ScmObject> arg = *it;

		// We have removed the argument at this place before and it should now be in _result
		if (arg == nullptr)
		{
			arg = _result;
		}

		switch (arg->getType())
		{
		case ScmObjectType::FLOAT:
		{
			double val = static_pointer_cast<ScmObject_Float>(arg)->getValue();

			if (lastRun)
			{
				floatProduct *= val;
			}
			else
			{
				floatProduct /= val;
			}
		}
		break;
		case ScmObjectType::INT:
		{
			long long val = static_pointer_cast<ScmObject_Integer>(arg)->getValue();

			if (lastRun)
			{
				floatProduct *= val;
			}
			else
			{
				floatProduct /= val;
			}
		}
		break;
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<ScmObject_Symbol>(arg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in divide. Symbol " + *static_pointer_cast<ScmObject_Symbol>(arg)->getName() + " is not defined.");
			}

			switch (obj->getType())
			{
			case ScmObjectType::FLOAT:
			{
				double val = static_pointer_cast<const ScmObject_Float>(obj)->getValue();

				if (lastRun)
				{
					floatProduct *= val;
				}
				else
				{
					floatProduct /= val;
				}
			}
			break;
			case ScmObjectType::INT:
			{
				long long val = static_pointer_cast<const ScmObject_Integer>(obj)->getValue();

				if (lastRun)
				{
					floatProduct *= val;
				}
				else
				{
					floatProduct /= val;
				}
			}
			break;
			default:
				return returnError(_callStack, _result, "Error while trying to execute built-in divide.");
			}
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(arg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgs(argIndex, make_shared<ScmObject_Float>(floatProduct));
			return;
		}
		break;
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in divide.");
		}
	}

	return endExecution(_callStack, _result, make_shared<ScmObject_Float>(floatProduct));
}

inline void execBuiltInCons(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 2)
	{
		return returnError(_callStack, _result, "Wrong argument count supplied for 'cons'. 'cons' takes exactly two arguments.");
	}

	std::shared_ptr<const ScmObject> car;
	std::shared_ptr<const ScmObject> cdr;
	int curArgIndex = 0;
	for (auto arg : *args)
	{
		std::shared_ptr<ScmObject> curArg = nullptr;
		if (arg == nullptr)
		{
			curArg = _result;
			currentExec->replaceArgAt(curArgIndex, curArg);
		}
		else
		{
			curArg = arg;
		}

		switch (curArg->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>(curArg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in cons. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			if (curArgIndex == 0)
				car = obj;
			else
				cdr = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(curArg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(curArgIndex, nullptr);
			return;
		}
		break;
		default:
		{
			if (curArgIndex == 0)
				car = curArg;
			else
				cdr = curArg;
		}
		break;
		}

		++curArgIndex;
	}

	return endExecution(_callStack, _result, make_shared<ScmObject_Cons>(car, cdr));
}

inline void execBuiltInCar(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "Wrong argument count supplied for 'car'. 'car' takes exactly one argument.");
	}

	std::shared_ptr<const ScmObject> curArg = nullptr;
	if ((*args)[0] == nullptr)
	{
		curArg = _result;
	}
	else
	{
		curArg = (*args)[0];
	}

	switch (curArg->getType())
	{
	case ScmObjectType::SYMBOL:
	{
		shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>(curArg));

		if (obj == nullptr)
		{
			return returnError(_callStack, _result, "Error while trying to execute built-in car. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
		}

		curArg = obj;
	}
	break;
	case ScmObjectType::FUNCTION_CALL:
	{
		shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(curArg);
		_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
		currentExec->replaceArgAt(0, nullptr);
		return;
	}
	break;
	default:
		break;
	}

	if (curArg->getType() == ScmObjectType::CONS)
	{
		return endExecution(_callStack, _result, const_pointer_cast<ScmObject>(static_pointer_cast<const ScmObject_Cons>(curArg)->car()));
	}
	else
	{
		return returnError(_callStack, _result, "Argument passed to 'car' did not evaluate to cons. Please make sure to pass a cons object.");
	}
}

inline void execBuiltInCdr(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "Wrong argument count supplied for 'cdr'. 'cdr' takes exactly one argument.");
	}

	std::shared_ptr<const ScmObject> curArg = nullptr;
	if ((*args)[0] == nullptr)
	{
		curArg = _result;
	}
	else
	{
		curArg = (*args)[0];
	}

	switch (curArg->getType())
	{
	case ScmObjectType::SYMBOL:
	{
		shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>(curArg));

		if (obj == nullptr)
		{
			return returnError(_callStack, _result, "Error while trying to execute built-in cdr. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
		}

		curArg = obj;
	}
	break;
	case ScmObjectType::FUNCTION_CALL:
	{
		shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(curArg);
		_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
		currentExec->replaceArgAt(0, nullptr);
		return;
	}
	break;
	default:
		break;
	}

	if (curArg->getType() == ScmObjectType::CONS)
	{
		return endExecution(_callStack, _result, const_pointer_cast<ScmObject>(static_pointer_cast<const ScmObject_Cons>(curArg)->cdr()));
	}
	else
	{
		return returnError(_callStack, _result, "Argument passed to 'cdr' did not evaluate to cons. Please make sure to pass a cons object.");
	}
}

inline void execBuiltInEquals(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 2)
	{
		return returnError(_callStack, _result, "Wrong argument count supplied for equals (eq?). Equals takes exactly two arguments.");
	}

	shared_ptr<const ScmObject> firstArg = nullptr;

	if ((*args)[0] == nullptr)
	{
		firstArg = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in equals. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			firstArg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		break;
		default:
			firstArg = (*args)[0];
			break;
		}
	}

	shared_ptr<const ScmObject> secondArg = nullptr;

	if ((*args)[1] == nullptr)
	{
		secondArg = _result;
	}
	else
	{
		switch ((*args)[1]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[1]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in equals. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			secondArg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[1]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(1, nullptr);
			return;
		}
		break;
		default:
			secondArg = (*args)[1];
			break;
		}
	}

	return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(firstArg->equals(&*secondArg)));
}

inline void execBuiltInGreaterThan(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 2)
	{
		return returnError(_callStack, _result, "Wrong argument count supplied for greater than (>). '>' takes exactly two arguments.");
	}

	shared_ptr<const ScmObject> firstArg = nullptr;

	if ((*args)[0] == nullptr)
	{
		firstArg = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in '>'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			firstArg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		break;
		default:
			firstArg = (*args)[0];
			break;
		}
	}

	union {
		long long intVal;
		double floatVal;
	} firstArgVal = { 0 };
	bool useFloatVal = false;

	switch (firstArg->getType())
	{
	case ScmObjectType::INT:
	{
		firstArgVal.intVal = static_pointer_cast<const ScmObject_Integer>(firstArg)->getValue();
		useFloatVal = false;
	}
	break;
	case ScmObjectType::FLOAT:
	{
		firstArgVal.floatVal = static_pointer_cast<const ScmObject_Float>(firstArg)->getValue();
		useFloatVal = true;
	}
	break;
	default:
		return returnError(_callStack, _result, "Greater than (>) only supports types float and integer. Please check the type of the first argument.");
	}

	shared_ptr<const ScmObject> secondArg = nullptr;

	if ((*args)[1] == nullptr)
	{
		secondArg = _result;
	}
	else
	{
		switch ((*args)[1]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[1]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in '>'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			secondArg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[1]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(1, nullptr);
			return;
		}
		break;
		default:
			secondArg = (*args)[1];
			break;
		}
	}

	switch (secondArg->getType())
	{
	case ScmObjectType::INT:
	{
		long long val = static_pointer_cast<const ScmObject_Integer>(secondArg)->getValue();

		if (useFloatVal)
		{
			return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(firstArgVal.floatVal > val));
		}
		else
		{
			return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(firstArgVal.intVal > val));
		}
	}
	break;
	case ScmObjectType::FLOAT:
	{
		double val = static_pointer_cast<const ScmObject_Float>(secondArg)->getValue();

		if (useFloatVal)
		{
			return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(firstArgVal.floatVal > val));
		}
		else
		{
			return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(firstArgVal.intVal > val));
		}
	}
	break;
	default:
		return returnError(_callStack, _result, "Greater than (>) only supports types float and integer. Please check the type of the second argument.");
	}
}

inline void execBuiltInLessThan(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 2)
	{
		return returnError(_callStack, _result, "Wrong argument count supplied for less than (<). '<' takes exactly two arguments.");
	}

	shared_ptr<const ScmObject> firstArg = nullptr;

	if ((*args)[0] == nullptr)
	{
		firstArg = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in '<'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			firstArg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		break;
		default:
			firstArg = (*args)[0];
			break;
		}
	}

	union {
		long long intVal;
		double floatVal;
	} firstArgVal = { 0 };
	bool useFloatVal = false;

	switch (firstArg->getType())
	{
	case ScmObjectType::INT:
	{
		firstArgVal.intVal = static_pointer_cast<const ScmObject_Integer>(firstArg)->getValue();
		useFloatVal = false;
	}
	break;
	case ScmObjectType::FLOAT:
	{
		firstArgVal.floatVal = static_pointer_cast<const ScmObject_Float>(firstArg)->getValue();
		useFloatVal = true;
	}
	break;
	default:
		return returnError(_callStack, _result, "Less than (<) only supports types float and integer. Please check the type of the first argument.");
	}

	shared_ptr<const ScmObject> secondArg = nullptr;

	if ((*args)[1] == nullptr)
	{
		secondArg = _result;
	}
	else
	{
		switch ((*args)[1]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[1]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in '<'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			secondArg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[1]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(1, nullptr);
			return;
		}
		break;
		default:
			secondArg = (*args)[1];
			break;
		}
	}

	switch (secondArg->getType())
	{
	case ScmObjectType::INT:
	{
		long long val = static_pointer_cast<const ScmObject_Integer>(secondArg)->getValue();

		if (useFloatVal)
		{
			return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(firstArgVal.floatVal < val));
		}
		else
		{
			return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(firstArgVal.intVal < val));
		}
	}
	break;
	case ScmObjectType::FLOAT:
	{
		double val = static_pointer_cast<const ScmObject_Float>(secondArg)->getValue();

		if (useFloatVal)
		{
			return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(firstArgVal.floatVal < val));
		}
		else
		{
			return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(firstArgVal.intVal < val));
		}
	}
	break;
	default:
		return returnError(_callStack, _result, "Less than (<) only supports types float and integer. Please check the type of the second argument.");
	}
}

inline void execBuiltInStringQ(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "'string?' takes exactly one argument.");
	}

	shared_ptr<const ScmObject> arg = nullptr;

	if ((*args)[0] == nullptr)
	{
		arg = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in 'string?'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			arg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		default:
			arg = (*args)[0];
		}
	}

	switch (arg->getType())
	{
	case ScmObjectType::STRING:
		return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(true));
	default:
		return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(false));
	}
}

inline void execBuiltInNumberQ(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "'number?' takes exactly one argument.");
	}

	shared_ptr<const ScmObject> arg = nullptr;

	if ((*args)[0] == nullptr)
	{
		arg = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in 'number?'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			arg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		default:
			arg = (*args)[0];
		}
	}

	switch (arg->getType())
	{
	case ScmObjectType::INT:
	case ScmObjectType::FLOAT:
		return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(true));
	default:
		return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(false));
	}
}

inline void execBuiltInConsQ(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "'cons?' takes exactly one argument.");
	}

	shared_ptr<const ScmObject> arg = nullptr;

	if ((*args)[0] == nullptr)
	{
		arg = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in 'cons?'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			arg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		default:
			arg = (*args)[0];
		}
	}

	switch (arg->getType())
	{
	case ScmObjectType::CONS:
		return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(true));
	default:
		return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(false));
	}
}

inline void execBuiltInFunctionQ(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "'function?' takes exactly one argument.");
	}

	shared_ptr<const ScmObject> arg = nullptr;

	if ((*args)[0] == nullptr)
	{
		arg = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in 'function?'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			arg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		default:
			arg = (*args)[0];
		}
	}

	switch (arg->getType())
	{
	case ScmObjectType::FUNCTION_DEFINITION:
		return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(static_pointer_cast<const ScmObject_FunctionDefinition>(arg)->getFunctionType() != ScmObject_FunctionDefinition::FunctionType::CUSTOM));
	default:
		return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(false));
	}
}

inline void execBuiltInUserDefinedFunctionQ(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "'udf?' takes exactly one argument.");
	}

	shared_ptr<const ScmObject> arg = nullptr;

	if ((*args)[0] == nullptr)
	{
		arg = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in 'udf?'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			arg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		default:
			arg = (*args)[0];
		}
	}

	switch (arg->getType())
	{
	case ScmObjectType::FUNCTION_DEFINITION:
		return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(static_pointer_cast<const ScmObject_FunctionDefinition>(arg)->getFunctionType() == ScmObject_FunctionDefinition::FunctionType::CUSTOM));
	default:
		return endExecution(_callStack, _result, make_shared<ScmObject_Bool>(false));
	}
}

inline void execBuiltInPrint(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	int curArgIndex = 0;
	for (auto arg : *args)
	{
		shared_ptr<ScmObject> curArg = nullptr;

		if (arg == nullptr)
		{
			curArg = _result;
		}
		else
		{
			curArg = arg;
		}

		switch (curArg->getType())
		{
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(curArg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->removeArgsBefore(curArgIndex, nullptr);
			return;
		}
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>(curArg));

			if (obj == nullptr)
			{
				cout << endl;
				return returnError(_callStack, _result, "Error while trying to execute built-in 'print'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			cout << obj->getOutputString();
		}
		break;
		default:
			cout << curArg->getOutputString();
			break;
		}

		++curArgIndex;
	}

	cout << endl;

	return endExecution(_callStack, _result, nullptr);
}

inline void execBuiltInDisplay(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	int curArgIndex = 0;
	for (auto arg : *args)
	{
		shared_ptr<ScmObject> curArg = nullptr;

		if (arg == nullptr)
		{
			curArg = _result;
		}
		else
		{
			curArg = arg;
		}

		switch (curArg->getType())
		{
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(curArg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->removeArgsBefore(curArgIndex, nullptr);
			return;
		}
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>(curArg));

			if (obj == nullptr)
			{
				cout << endl;
				return returnError(_callStack, _result, "Error while trying to execute built-in 'print'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			cout << obj->getOutputString();
		}
		break;
		default:
			cout << curArg->getDisplayString();
			break;
		}

		++curArgIndex;
	}

	cout << endl;

	return endExecution(_callStack, _result, nullptr);
}

inline void execBuiltInFunctionBody(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "The amount of arguments supplied to 'function-body' was not equal to two. Define only accepts exactly one arguments.");
	}

	std::shared_ptr<const ScmObject> arg = nullptr;

	if ((*args)[0] == nullptr)
	{
		arg = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in 'function-body'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			arg = obj;
		}
		break;
		default:
			arg = (*args)[0];
			break;
		}
	}

	switch (arg->getType())
	{
	case ScmObjectType::FUNCTION_DEFINITION:
		return endExecution(_callStack, _result, static_pointer_cast<const ScmObject_FunctionDefinition>(arg)->getFunctionBody());
	default:
		return returnError(_callStack, _result, "Argument passed to 'function-body' does not evaluate to a lambda.");
	}
}

inline void execBuiltInFunctionArgList(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "The amount of arguments supplied to 'function-arglist' was not equal to two. Define only accepts exactly one arguments.");
	}

	std::shared_ptr<const ScmObject> arg = nullptr;

	if ((*args)[0] == nullptr)
	{
		arg = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in 'function-arglist'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			arg = obj;
		}
		break;
		default:
			arg = (*args)[0];
			break;
		}
	}

	switch (arg->getType())
	{
	case ScmObjectType::FUNCTION_DEFINITION:
		return endExecution(_callStack, _result, static_pointer_cast<const ScmObject_FunctionDefinition>(arg)->getFunctionArgList());
	default:
		return returnError(_callStack, _result, "Argument passed to 'function-arglist' does not evaluate to a lambda.");
	}
}

inline void execBuiltInReadLine(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 0)
	{
		return returnError(_callStack, _result, "The amount of arguments supplied to 'read-line' was not equal to zero. 'read-line' accepts no arguments.");
	}

	string input;

	cin >> input;

	return endExecution(_callStack, _result, make_shared<ScmObject_String>(input));
}

inline void execBuiltInLoad(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "Them amount of arguments supplied to 'load' was not equal to one. 'load' takes exactly one argument.");
	}

	std::shared_ptr<const ScmObject> path;

	if ((*args)[0] == nullptr)
	{
		path = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in 'load'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			path = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
		break;
		case ScmObjectType::STRING:
		{
			path = (*args)[0];
		}
		break;
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in 'load'. Wrong argument type. Load requires one argument of type string.");
		}

		if (path->getType() != ScmObjectType::STRING)
		{
			return returnError(_callStack, _result, "Error while trying to execute built-in 'load'. Wrong argument type. Load requires one argument of type string.");
		}

		string data = "";
		ifstream file;
		file.open(*static_pointer_cast<const ScmObject_String>(path)->getValue());
		if (!file.is_open())
		{
			return returnError(_callStack, _result, "File at path " + path->getOutputString() + " does not exist.");
		}

		string curLine;

		int curLineNumber = 0;

		do
		{
			std::getline(file, curLine);
			++curLineNumber;

			// Check for comments and remove them if found.
			auto index = curLine.find(";");
			if (index != -1)
			{
				if (index == 0)
				{
					continue;
				}
				else
				{
					curLine = curLine.substr(0, index);
				}
			}

			data += curLine;
			// Create extra data string, so that Reader::ReadNextSymbol(...) can manipulate it.
			string dataToGive = data;

			shared_ptr<ScmObject> readObj = Reader::ReadNextSymbol(dataToGive);

			if (readObj == nullptr)
			{
				if (file.eof())
				{
					return returnError(_callStack, _result, "End of file reached too early. Probably missing one or more ')'.");
				}
				else
				{
					continue;
				}
			}

			data = dataToGive;

			switch (readObj->getType())
			{
			case ScmObjectType::FUNCTION_CALL:
			{
				execTramp(static_pointer_cast<const ScmObject_FunctionCall>(readObj)->createFunctionExecution(nullptr, nullptr));
			}
			break;
			case ScmObjectType::INTERNAL_ERROR:
			{
				return returnError(_callStack, _result, "Error in line " + to_string(curLineNumber) + ".");
			}
			break;
			default:
				break;
			}
		} while (!file.eof());

		return endExecution(_callStack, _result, make_shared<ScmObject_String>("Executed"));
	}
}

inline void execSyntaxQuote(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 1)
	{
		return returnError(_callStack, _result, "The amount of arguments supplied to 'quote' was not equal to one. 'quote' only accepts exactly one argument.");
	}

	switch ((*args)[0]->getType())
	{
	case ScmObjectType::FUNCTION_CALL:
		return endExecution(_callStack, _result, static_pointer_cast<ScmObject_FunctionCall>((*args)[0])->makeCons());
	case ScmObjectType::FUNCTION_DEFINITION:
		throw new exception("Received function definition in 'quote'. This is deprecated and should not happen anymore.");
		//return endExecution(_callStack, _result, static_pointer_cast<ScmObject_FunctionDefinition>((*args)[0])->makeCons());
	default:
		return endExecution(_callStack, _result, std::make_shared<ScmObject_QuoteSymbol>(static_pointer_cast<ScmObject_Symbol>((*args)[0])->getOutputString()));
	}
}

inline void execSyntaxLambda(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() < 2)
	{
		return returnError(_callStack, _result, "Wrong number of arguments when calling 'lambda'. 'lambda' expects at least two arguments (parameter list, instructions).");
	}

	if ((*args)[0]->getType() != ScmObjectType::FUNCTION_CALL)
	{
		return returnError(_callStack, _result, "Call to 'lambda' expected argument list but got data of different type. Please supply the argument list first.");
	}

	std::shared_ptr<ScmObject_FunctionCall> functionCallParamterObj = static_pointer_cast<ScmObject_FunctionCall>((*args)[0]);
	const std::vector<shared_ptr<ScmObject>>* functionArgs = functionCallParamterObj->getFunctionArgs();
	std::vector<shared_ptr<ScmObject_Symbol>> parameterList;
	parameterList.reserve(functionArgs->size() + 1);

	parameterList.push_back(const_pointer_cast<ScmObject_Symbol>(functionCallParamterObj->getFunctionSymbol()));

	for (auto it : *functionArgs)
	{
		if (it->getType() != ScmObjectType::SYMBOL)
		{
			return returnError(_callStack, _result, "Expected only symbols in paramter list of lambda expression but got different type.");
		}
		else
		{
			parameterList.push_back(static_pointer_cast<ScmObject_Symbol>(it));
		}
	}

	std::vector<shared_ptr<ScmObject>> functionCalls(args->begin() + 1, args->end());

	return endExecution(_callStack, _result, make_shared<ScmObject_FunctionDefinition>(parameterList, functionCalls, currentExec->environment, currentExec->closureEnvironment));
}

inline void execSyntaxDefine(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() < 2)
	{
		return returnError(_callStack, _result, "The amount of arguments supplied to define was less than two. Define only accepts two or more (in case of function definitions (lambda-shorthand)) arguments.");
	}

	shared_ptr<const ScmObject> symbolValue = nullptr;

	if ((*args)[1] == nullptr)
	{
		symbolValue = _result;
		// Prevent our new symbol value from being deleted later on.
		_result = nullptr;
	}
	else
	{
		switch ((*args)[1]->getType())
		{
		case ScmObjectType::FUNCTION_CALL:
		{
			// Prevent our lambda shorthand from being tampered with.
			if ((*args)[0]->getType() != ScmObjectType::FUNCTION_CALL && (*args)[0]->getType() != ScmObjectType::FUNCTION_DEFINITION)
			{
				shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[1]);
				_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
				currentExec->replaceArgAt(1, nullptr);
				return;
			}
			else
			{
				symbolValue = (*args)[1];
			}
		}
		default:
			symbolValue = (*args)[1];
			break;
		}
	}

	shared_ptr<const ScmObject_Symbol> symbolToDefine = nullptr;

	switch ((*args)[0]->getType())
	{
	case ScmObjectType::SYMBOL:
	{
		// If the symbol value is itself a symbol, we first have to get the actual value.
		if (symbolValue->getType() == ScmObjectType::SYMBOL)
		{
			symbolValue = lookupInEnv(currentExec, static_pointer_cast<const ScmObject_Symbol>(symbolValue));
		}

		symbolToDefine = static_pointer_cast<ScmObject_Symbol>((*args)[0]);
	}
	break;
	// Lambda shorthand.
	case ScmObjectType::FUNCTION_CALL:
	{
		auto funcCall = static_pointer_cast<ScmObject_FunctionCall>((*args)[0]);

		symbolToDefine = funcCall->getFunctionSymbol();
		auto args = funcCall->getFunctionArgs();
		std::vector<std::shared_ptr<ScmObject_Symbol>> unconstArgs;
		unconstArgs.reserve(args->size());

		for (auto arg : *args)
		{
			if (arg->getType() != ScmObjectType::SYMBOL)
			{
				return returnError(_callStack, _result, "Parameter of short-hand lambda was not a symbol.");
			}
			else
			{
				unconstArgs.push_back(const_pointer_cast<ScmObject_Symbol>(static_pointer_cast<const ScmObject_Symbol>(arg)));
			}
		}

		switch (symbolValue->getType())
		{
		case ScmObjectType::FUNCTION_CALL:
			{
				args = currentExec->getFunctionArgs();
				std::vector<shared_ptr<ScmObject>> functionCalls(args->begin() + 1, args->end());
				symbolValue = std::make_shared<ScmObject_FunctionDefinition>(unconstArgs, functionCalls, currentExec->environment, currentExec->closureEnvironment);
			}
			break;
		case ScmObjectType::FUNCTION_DEFINITION:
			throw new exception("Gotten a function definition as value in a define lambda shorthand. This may never happen and represents an error in code.");
			//symbolValue = std::make_shared<ScmObject_FunctionDefinition>(unconstArgs, static_pointer_cast<ScmObject_FunctionDefinition>(symbolValue));
			break;
		default:
			return returnError(_callStack, _result, "Defining lambda failed. Function body was not a function or lambda.");
		}
	}
	break;
	default:
		return returnError(_callStack, _result, "The first argument of call to define was no symbol.");
	}

	if (currentExec->closureEnvironment != nullptr)
	{
		currentExec->closureEnvironment->addSymbol(symbolToDefine, symbolValue);
	}
	else
	{
		globalEnvironment->addSymbol(symbolToDefine, symbolValue);
	}
	return endExecution(_callStack, _result, const_pointer_cast<ScmObject_Symbol>(symbolToDefine));
}

inline void execSyntaxIf(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 3)
	{
		return returnError(_callStack, _result, "Wrong number of arguments for if. Syntax is (if <cond> <true_expr> <false_expr>).");
	}

	shared_ptr<const ScmObject> condition = nullptr;

	if ((*args)[0] == nullptr)
	{
		condition = _result;
	}
	else
	{
		switch ((*args)[0]->getType())
		{
		case ScmObjectType::BOOL:
		{
			condition = (*args)[0];
		}
		break;
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<ScmObject_Symbol>((*args)[0]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute 'if'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			switch (obj->getType())
			{
			case ScmObjectType::BOOL:
			{
				condition = obj;
			}
			break;
#ifdef STRICT_IF
			default:
				return returnError(_callStack, _result, "Error in 'if'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not of type boolean.");
#endif
			}
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[0]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(0, nullptr);
			return;
		}
#ifdef STRICT_IF
		default:
			return returnError(_callStack, _result, "Error in 'if'. First expression of if statement must be a boolean or evaluate to a boolean.");
		}
	}

	if (condition->getType() != ScmObjectType::BOOL)
	{
		return returnError(_callStack, _result, "Error in 'if'. First expression of if statement must be a boolean or evaluate to a boolean.");
	}

	bool execTrue = static_pointer_cast<const ScmObject_Bool>(condition)->getValue();
#else
		default:
			condition = nullptr;
			break;
}
	}

	bool execTrue = true;

	if (condition != nullptr && condition->getType() == ScmObjectType::BOOL)
	{
		execTrue = static_pointer_cast<const ScmObject_Bool>(condition)->getValue();
	}
#endif

	shared_ptr<ScmObject> usedExpr = (*args)[execTrue ? 1 : 2];

	switch (usedExpr->getType())
	{
	case ScmObjectType::FUNCTION_CALL:
	{
		// Replace the call to if with the call to our evaluated expression.
		_callStack.pop();
		shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(usedExpr);
		_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
	}
	break;
	default:
		return endExecution(_callStack, _result, usedExpr);
	}
}

inline void execSyntaxSet(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() != 2)
	{
		return returnError(_callStack, _result, "Wrong argument count for 'set!'. 'set!' takes exactly two arguments.");
	}

	if ((*args)[0]->getType() != ScmObjectType::SYMBOL)
	{
		return returnError(_callStack, _result, "First argument passed to 'set!' was not a symbol. This is invalid.");
	}

	std::shared_ptr<ScmObject_Symbol> symbol = static_pointer_cast<ScmObject_Symbol>((*args)[0]);

	shared_ptr<const ScmObject> valueToSet = nullptr;

	if ((*args)[1] == nullptr)
	{
		valueToSet = _result;
	}
	else
	{
		switch ((*args)[1]->getType())
		{
		case ScmObjectType::FUNCTION_CALL:
		{
			shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[1]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(1, nullptr);
			return;
		}
		case ScmObjectType::SYMBOL:
		{
			shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<ScmObject_Symbol>((*args)[1]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute 'set!'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
			}

			valueToSet = obj;
		}
		break;
		default:
		{
			valueToSet = (*args)[1];
		}
		break;
		}
	}

	bool setSuccessful = false;
	setSuccessful = currentExec->closureEnvironment->setSymbol(symbol, valueToSet);
	if (!setSuccessful)
	{
		setSuccessful = currentExec->environment->setSymbol(symbol, valueToSet);
	}

	if (setSuccessful)
	{
		return endExecution(_callStack, _result, symbol);
	}
	else
	{
		return returnError(_callStack, _result, "Setting symbol " + *symbol->getName() + " failed. Symbol has not been defined yet.");
	}
}

inline void execSyntaxBegin(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();

	if (args->size() == 0)
	{
		return returnError(_callStack, _result, "Begin called with 0 expressions. This is invalid.");
	}

	int argIndex = 0;
	for (auto it = args->begin(); it != args->end(); ++it, ++argIndex)
	{
		bool lastArg = (it + 1) == args->end();

		if (*it == nullptr)
		{
			if (lastArg)
			{
				return endExecution(_callStack, _result, _result);
			}
			else
			{
				// If we are not at the last arg but have our value from a result we can just continue.
				continue;
			}
		}
		else
		{
			switch ((*it)->getType())
			{
			case ScmObjectType::SYMBOL:
			{
				if (lastArg)
				{
					shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, static_pointer_cast<ScmObject_Symbol>((*args)[0]));

					if (obj == nullptr)
					{
						return returnError(_callStack, _result, "Error while trying to execute 'begin'. Symbol " + *static_pointer_cast<ScmObject_Symbol>((*args)[0])->getName() + " is not defined.");
					}

					return endExecution(_callStack, _result, const_pointer_cast<ScmObject>(obj));
				}
				else
				{
					continue;
				}
			}
			break;
			case ScmObjectType::FUNCTION_CALL:
			{
				shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>((*args)[argIndex]);
				_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
				currentExec->replaceArgAt(argIndex, nullptr);
				return;
			}
			default:
				if (lastArg)
				{
					return endExecution(_callStack, _result, *it);
				}
				break;
			}
		}
	}
}

inline void execCustomFunction(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	const std::vector<shared_ptr<ScmObject>>* args = currentExec->getFunctionArgs();
	std::vector<shared_ptr<ScmObject>> evaluatedArgs;
	evaluatedArgs.reserve(args->size());

	// Before we actually call our function, we need to make sure our arguments are no function calls.
	int argIndex = 0;

	for (auto it = args->begin(); it != args->end(); ++it, ++argIndex)
	{
		shared_ptr<ScmObject> arg = nullptr;

		if (*it == nullptr)
		{
			arg = _result;
			currentExec->replaceArgAt(argIndex, arg);
		}
		else
		{
			switch ((*it)->getType())
			{
			case ScmObjectType::FUNCTION_CALL:
			{
				shared_ptr<const ScmObject_FunctionCall> funcCall = static_pointer_cast<const ScmObject_FunctionCall>(*it);
				_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
				currentExec->replaceArgAt(argIndex, nullptr);
				return;
			}
			break;
			default:
			{
				arg = *it;
			}
			break;
			}
		}

		evaluatedArgs.push_back(arg);
	}

	shared_ptr<const ScmObject> obj = lookupInEnv(currentExec, currentExec->functionCall->getFunctionSymbol());

	if (obj == nullptr)
	{
		return returnError(_callStack, _result, "Function " + *currentExec->functionCall->getFunctionSymbol()->getName() + " is not defined.");
	}
	else
	{
		switch (obj->getType())
		{
		case ScmObjectType::FUNCTION_DEFINITION:
		{
			vector<std::shared_ptr<ScmObject>> funcs = static_pointer_cast<const ScmObject_FunctionDefinition>(obj)->getExecutable(evaluatedArgs);
			
			while (currentExec->currentUdfFunctionIndex < funcs.size())
			{
				switch (funcs[currentExec->currentUdfFunctionIndex]->getType())
				{
				case ScmObjectType::FUNCTION_EXECUTION:
				{
					_callStack.push(static_pointer_cast<ScmObject_FunctionExecution>(funcs[currentExec->currentUdfFunctionIndex]));
					++currentExec->currentUdfFunctionIndex;
					return;
				}
				break;
				case ScmObjectType::INTERNAL_ERROR:
				{
					return returnError(_callStack, _result, static_pointer_cast<ScmObject_InternalError>(funcs[currentExec->currentUdfFunctionIndex])->getMessage());
				}
				break;
				default:
					// Not needed, unless simple data should be printed.
					//cout << funcs[currentExec->currentUdfFunctionIndex]->getOutputString();

					++currentExec->currentUdfFunctionIndex;;

					// Set the last instruction as our result. (If this is the last instruction.)
					if (currentExec->currentUdfFunctionIndex >= funcs.size())
					{
						_result = funcs[currentExec->currentUdfFunctionIndex];
					}

					break;
				}
			}
		}
		break;
		default:
			return returnError(_callStack, _result, "Tried calling user defined function but symbol supplied is not bound to a user defined function.");
		}

		return endExecution(_callStack, _result, _result);
	}
}

void execActual(std::stack<std::shared_ptr<ScmObject_FunctionExecution>>& _callStack, shared_ptr<ScmObject>& _result)
{
	std::shared_ptr<ScmObject_FunctionExecution> currentExec = _callStack.top();

	shared_ptr<const ScmObject> func = lookupInEnv(currentExec, currentExec->functionCall->getFunctionSymbol());

	if (func == nullptr || func->getType() != ScmObjectType::FUNCTION_DEFINITION)
	{
		_result = make_shared<ScmObject_InternalError>("Tried calling function " + *currentExec->functionCall->getFunctionSymbol()->getName() + " but it has not been defined.");
		while (_callStack.size() > 0)
		{
			_callStack.pop();
		}
		return;
	}

	shared_ptr<const ScmObject_FunctionDefinition> funcDef = static_pointer_cast<const ScmObject_FunctionDefinition>(func);

	switch (funcDef->getFunctionType())
	{
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_ADD:
		return execBuiltInAdd(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_SUBTRACT:
		return execBuiltInSubtract(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_MULTIPLY:
		return execBuiltInMultiply(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_DIVIDE:
		return execBuiltInDivide(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_CONS:
		return execBuiltInCons(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_CAR:
		return execBuiltInCar(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_CDR:
		return execBuiltInCdr(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_EQUALS:
		return execBuiltInEquals(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_GT:
		return execBuiltInGreaterThan(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_LT:
		return execBuiltInLessThan(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_STRINGQ:
		return execBuiltInStringQ(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_NUMBERQ:
		return execBuiltInNumberQ(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_CONSQ:
		return execBuiltInConsQ(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_FUNCTIONQ:
		return execBuiltInFunctionQ(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_USER_DEFINED_FUNCTIONQ:
		return execBuiltInUserDefinedFunctionQ(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_PRINT:
		return execBuiltInPrint(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_DISPLAY:
		return execBuiltInDisplay(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_FUNCTION_BODY:
		return execBuiltInFunctionBody(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_FUNCTION_ARG_LIST:
		return execBuiltInFunctionArgList(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_READ_LINE:
		return execBuiltInReadLine(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_LOAD:
		return execBuiltInLoad(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::SYNTAX_QUOTE:
		return execSyntaxQuote(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::SYNTAX_LAMBDA:
		return execSyntaxLambda(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::SYNTAX_DEFINE:
		return execSyntaxDefine(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::SYNTAX_IF:
		return execSyntaxIf(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::SYNTAX_SET:
		return execSyntaxSet(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::SYNTAX_BEGIN:
		return execSyntaxBegin(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::CUSTOM:
		return execCustomFunction(_callStack, _result);
	default:
		break;
	}
}

inline shared_ptr<ScmObject> execTramp(std::shared_ptr<ScmObject_FunctionExecution> _functionCall)
{
	std::stack<std::shared_ptr<ScmObject_FunctionExecution>> callStack;
	callStack.push(_functionCall);
	std::shared_ptr<ScmObject> result = nullptr;

	while (callStack.size() > 0)
	{
		execActual(callStack, result);
	}

	return result;
}

shared_ptr<ScmObject> exec(shared_ptr<ScmObject_FunctionExecution> _funcExec)
{
#ifdef MEASURE_EXECUTION_TIME
	auto start = std::chrono::high_resolution_clock::now();
#endif

	shared_ptr<ScmObject> result = execTramp(_funcExec);

#ifdef MEASURE_EXECUTION_TIME
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsedTime = end - start;

	std::cout << "Elapsed time for function call: " << (elapsedTime.count() * 1000) << " ms\n";
#endif

	return result;
}