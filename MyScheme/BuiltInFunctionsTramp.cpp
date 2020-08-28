#define MEASURE_EXECUTION_TIME
#define STRICT_IF

#include "BuiltInFunctionsTramp.h"
#include "ScmObjectIncludes.h"
#include <stack>
#ifdef MEASURE_EXECUTION_TIME
#include <chrono>
#include <iostream>
#endif

const ScmObject* lookupInEnv(const ScmObject_FunctionExecution* _exec, const ScmObject_Symbol* _symbol)
{
	const ScmObject* boundSymbol = nullptr;

	if (_exec->closureEnvironment != nullptr)
	{
		boundSymbol = _exec->closureEnvironment->getSymbol(_symbol);
	}
	if (boundSymbol == nullptr)
	{
		boundSymbol = _exec->environment->getSymbol(_symbol);
	}

	return boundSymbol;
}

inline void returnError(std::stack<ScmObject_FunctionExecution*>& _stack, ScmObject*& _result, std::string _message)
{
	while (_stack.size() > 0)
	{
		_stack.pop();
	}

	delete _result;
	_result = new ScmObject_InternalError(_message);
}

inline void endExecution(std::stack<ScmObject_FunctionExecution*>& _stack, ScmObject*& _result, ScmObject* _newResult)
{
	delete _stack.top();
	_stack.pop();

	delete _result;
	_result = _newResult;
}

inline void execBuiltInAdd(std::stack<ScmObject_FunctionExecution*>& _callStack, ScmObject*& _result)
{
	ScmObject_FunctionExecution* currentExec = _callStack.top();

	const std::vector<ScmObject*>* args = currentExec->getFunctionArgs();

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
		ScmObject* arg = *it;

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

				floatSum += static_cast<ScmObject_Float*>(arg)->getValue();
			}
			break;
		case ScmObjectType::INT:
			{
				long long val = static_cast<ScmObject_Integer*>(arg)->getValue();

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
				const ScmObject* obj = lookupInEnv(currentExec, static_cast<ScmObject_Symbol*>(arg));

				if (obj == nullptr)
				{
					return returnError(_callStack, _result, "Error while trying to execute built-in add. Symbol " + *static_cast<ScmObject_Symbol*>(arg)->getName() + " is not defined.");
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

						floatSum += static_cast<const ScmObject_Float*>(obj)->getValue();
					}
					break;
				case ScmObjectType::INT:
					{
						long long val = static_cast<const ScmObject_Integer*>(obj)->getValue();

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
				const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>(arg);
				_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
				currentExec->replaceArgs(argIndex, useFloatSum ? new ScmObject_Float(floatSum) : static_cast<ScmObject*>(new ScmObject_Integer(intSum)));
				return;
			}
			break;
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in add.");
		}
	}

	if (useFloatSum)
	{
		return endExecution(_callStack, _result, new ScmObject_Float(floatSum));
	}
	else
	{
		return endExecution(_callStack, _result, new ScmObject_Integer(intSum));
	}
}

inline void execBuiltInSubtract(std::stack<ScmObject_FunctionExecution*>& _callStack, ScmObject*& _result)
{
	ScmObject_FunctionExecution* currentExec = _callStack.top();

	const std::vector<ScmObject*>* args = currentExec->getFunctionArgs();

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
			return endExecution(_callStack, _result, new ScmObject_Float(-static_cast<ScmObject_Float*>(curArg)->getValue()));
		case ScmObjectType::INT:
			return endExecution(_callStack, _result, new ScmObject_Integer(-static_cast<ScmObject_Integer*>(curArg)->getValue()));
		case ScmObjectType::SYMBOL:
			{
				const ScmObject* obj = lookupInEnv(currentExec, static_cast<ScmObject_Symbol*>(curArg));

				if (obj == nullptr)
				{
					return returnError(_callStack, _result, "Error while trying to execute built-in subtract. Symbol " + *static_cast<ScmObject_Symbol*>(curArg)->getName() + " is not defined.");
				}

				switch (obj->getType())
				{
				case ScmObjectType::FLOAT:
					return endExecution(_callStack, _result, new ScmObject_Float(-static_cast<const ScmObject_Float*>(obj)->getValue()));
				case ScmObjectType::INT:
					return endExecution(_callStack, _result, new ScmObject_Integer(-static_cast<const ScmObject_Integer*>(obj)->getValue()));
				default:
					return returnError(_callStack, _result, "Error while trying to execute built-in subtract. Wrong argument type.");
				}
			}
			break;
		case ScmObjectType::FUNCTION_CALL:
			{
				const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>(curArg);
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

		ScmObject* arg = *it;

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

			double val = static_cast<ScmObject_Float*>(arg)->getValue();

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
			long long val = static_cast<ScmObject_Integer*>(arg)->getValue();

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
			const ScmObject* obj = lookupInEnv(currentExec, static_cast<ScmObject_Symbol*>(arg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in subtract. Symbol " + *static_cast<ScmObject_Symbol*>(arg)->getName() + " is not defined.");
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

				double val = static_cast<const ScmObject_Float*>(obj)->getValue();

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
				long long val = static_cast<const ScmObject_Integer*>(obj)->getValue();

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
			const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>(arg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgs(argIndex, useFloatSum ? new ScmObject_Float(floatSum) : static_cast<ScmObject*>(new ScmObject_Integer(intSum)));
			return;
		}
		break;
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in subtract.");
		}
	}

	if (useFloatSum)
	{
		return endExecution(_callStack, _result, new ScmObject_Float(floatSum));
	}
	else
	{
		return endExecution(_callStack, _result, new ScmObject_Integer(intSum));
	}
}

inline void execBuiltInMultiply(std::stack<ScmObject_FunctionExecution*>& _callStack, ScmObject*& _result)
{
	ScmObject_FunctionExecution* currentExec = _callStack.top();

	const std::vector<ScmObject*>* args = currentExec->getFunctionArgs();

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
		ScmObject* arg = *it;

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

			floatProduct *= static_cast<ScmObject_Float*>(arg)->getValue();
		}
		break;
		case ScmObjectType::INT:
		{
			long long val = static_cast<ScmObject_Integer*>(arg)->getValue();

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
			const ScmObject* obj = lookupInEnv(currentExec, static_cast<ScmObject_Symbol*>(arg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in multiply. Symbol " + *static_cast<ScmObject_Symbol*>(arg)->getName() + " is not defined.");
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

				floatProduct *= static_cast<const ScmObject_Float*>(obj)->getValue();
			}
			break;
			case ScmObjectType::INT:
			{
				long long val = static_cast<const ScmObject_Integer*>(obj)->getValue();

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
			const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>(arg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgs(argIndex, useFloatProduct ? new ScmObject_Float(floatProduct) : static_cast<ScmObject*>(new ScmObject_Integer(intProduct)));
			return;
		}
		break;
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in multiply.");
		}
	}

	if (useFloatProduct)
	{
		return endExecution(_callStack, _result, new ScmObject_Float(floatProduct));
	}
	else
	{
		return endExecution(_callStack, _result, new ScmObject_Integer(intProduct));
	}
}

inline void execBuiltInDivide(std::stack<ScmObject_FunctionExecution*>& _callStack, ScmObject*& _result)
{
	ScmObject_FunctionExecution* currentExec = _callStack.top();

	const std::vector<ScmObject*>* args = currentExec->getFunctionArgs();

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
			return endExecution(_callStack, _result, new ScmObject_Float(1.0 / static_cast<ScmObject_Float*>(curArg)->getValue()));
		case ScmObjectType::INT:
			return endExecution(_callStack, _result, new ScmObject_Float(1.0 / static_cast<ScmObject_Integer*>(curArg)->getValue()));
		case ScmObjectType::SYMBOL:
		{
			const ScmObject* obj = lookupInEnv(currentExec, static_cast<ScmObject_Symbol*>(curArg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in divide. Symbol " + *static_cast<ScmObject_Symbol*>(curArg)->getName() + " is not defined.");
			}

			switch (obj->getType())
			{
			case ScmObjectType::FLOAT:
				return endExecution(_callStack, _result, new ScmObject_Float(1.0 / static_cast<const ScmObject_Float*>(obj)->getValue()));
			case ScmObjectType::INT:
				return endExecution(_callStack, _result, new ScmObject_Float(1.0 / static_cast<const ScmObject_Integer*>(obj)->getValue()));
			default:
				return returnError(_callStack, _result, "Error while trying to execute built-in divide. Wrong argument type.");
			}
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>(curArg);
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

		ScmObject* arg = *it;

		// We have removed the argument at this place before and it should now be in _result
		if (arg == nullptr)
		{
			arg = _result;
		}

		switch (arg->getType())
		{
		case ScmObjectType::FLOAT:
		{
			double val = static_cast<ScmObject_Float*>(arg)->getValue();

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
			long long val = static_cast<ScmObject_Integer*>(arg)->getValue();

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
			const ScmObject* obj = lookupInEnv(currentExec, static_cast<ScmObject_Symbol*>(arg));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in divide. Symbol " + *static_cast<ScmObject_Symbol*>(arg)->getName() + " is not defined.");
			}

			switch (obj->getType())
			{
			case ScmObjectType::FLOAT:
			{
				double val = static_cast<const ScmObject_Float*>(obj)->getValue();

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
				long long val = static_cast<const ScmObject_Integer*>(obj)->getValue();

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
			const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>(arg);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgs(argIndex, new ScmObject_Float(floatProduct));
			return;
		}
		break;
		default:
			return returnError(_callStack, _result, "Error while trying to execute built-in divide.");
		}
	}

	return endExecution(_callStack, _result, new ScmObject_Float(floatProduct));
}

inline void execBuiltInEquals(std::stack<ScmObject_FunctionExecution*>& _callStack, ScmObject*& _result)
{
	ScmObject_FunctionExecution* currentExec = _callStack.top();

	const std::vector<ScmObject*>* args = currentExec->getFunctionArgs();

	if (args->size() != 2)
	{
		return returnError(_callStack, _result, "Wrong argument count supplied for equals (eq?). Equals takes exactly two arguments.");
	}

	const ScmObject* firstArg = nullptr;

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
				const ScmObject* obj = lookupInEnv(currentExec, static_cast<const ScmObject_Symbol*>((*args)[0]));

				if (obj == nullptr)
				{
					return returnError(_callStack, _result, "Error while trying to execute built-in equals. Symbol " + *static_cast<ScmObject_Symbol*>((*args)[0])->getName() + " is not defined.");
				}

				firstArg = obj;
			}
			break;
		case ScmObjectType::FUNCTION_CALL:
			{
				const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>((*args)[0]);
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

	const ScmObject* secondArg = nullptr;

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
			const ScmObject* obj = lookupInEnv(currentExec, static_cast<const ScmObject_Symbol*>((*args)[1]));

			if (obj == nullptr)
			{
				return returnError(_callStack, _result, "Error while trying to execute built-in equals. Symbol " + *static_cast<ScmObject_Symbol*>((*args)[0])->getName() + " is not defined.");
			}

			secondArg = obj;
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>((*args)[1]);
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

	return endExecution(_callStack, _result, new ScmObject_Bool(firstArg->equals(secondArg)));
}

inline void execSyntaxDefine(std::stack<ScmObject_FunctionExecution*>& _callStack, ScmObject*& _result)
{
	ScmObject_FunctionExecution* currentExec = _callStack.top();

	const std::vector<ScmObject*>* args = currentExec->getFunctionArgs();

	if (args->size() != 2)
	{
		return returnError(_callStack, _result, "The amount of arguments supplied to define was not equal to two. Define only accepts exactly two arguments.");
	}

	ScmObject* symbolValue = nullptr;

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
			const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>((*args)[1]);
			_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
			currentExec->replaceArgAt(1, nullptr);
			return;
		}
		default:
			symbolValue = (*args)[1];
			break;
		}
	}

	ScmObject_Symbol* symbolToDefine = nullptr;

	switch ((*args)[0]->getType())
	{
	case ScmObjectType::SYMBOL:
		{
			symbolToDefine = static_cast<ScmObject_Symbol*>((*args)[0]);
		}
		break;
	default:
		return returnError(_callStack, _result, "The first argument of call to define was no symbol.");
	}

	currentExec->environment->addSymbol(symbolToDefine, symbolValue->copy());
	return endExecution(_callStack, _result, symbolToDefine->copy());
}

inline void execSyntaxIf(std::stack<ScmObject_FunctionExecution*>& _callStack, ScmObject*& _result)
{
	ScmObject_FunctionExecution* currentExec = _callStack.top();

	const std::vector<ScmObject*>* args = currentExec->getFunctionArgs();

	if (args->size() != 3)
	{
		return returnError(_callStack, _result, "Wrong number of arguments for if. Syntax is (if <cond> <true_expr> <false_expr>).");
	}

	const ScmObject* condition = nullptr;

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
				const ScmObject* obj = lookupInEnv(currentExec, static_cast<ScmObject_Symbol*>((*args)[0]));

				if (obj == nullptr)
				{
					return returnError(_callStack, _result, "Error while trying to execute 'if'. Symbol " + *static_cast<ScmObject_Symbol*>((*args)[0])->getName() + " is not defined.");
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
					return returnError(_callStack, _result, "Error in 'if'. Symbol " + *static_cast<ScmObject_Symbol*>((*args)[0])->getName() + " is not of type boolean.");
#endif
				}
			}
			break;
		case ScmObjectType::FUNCTION_CALL:
			{
				const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>((*args)[0]);
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

	bool execTrue = static_cast<const ScmObject_Bool*>(condition)->getValue();
#else
		default:
			condition = nullptr;
			break;
		}
	}

	bool execTrue = true;

	if (condition != nullptr && condition->getType() == ScmObjectType::BOOL)
	{
		execTrue = static_cast<const ScmObject_Bool*>(condition)->getValue();
	}
#endif

	ScmObject* usedExpr = (*args)[execTrue ? 1 : 2];

	switch (usedExpr->getType())
	{
	case ScmObjectType::FUNCTION_CALL:
	{
		// Replace the call to if with the call to our evaluated expression.
		_callStack.pop();
		const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>(usedExpr);
		_callStack.push(funcCall->createFunctionExecution(currentExec->environment, currentExec->closureEnvironment));
	}
	break;
	default:
		return endExecution(_callStack, _result, usedExpr->copy());
	}
}

inline void execCustomFunction(std::stack<ScmObject_FunctionExecution*>& _callStack, ScmObject*& _result)
{
	ScmObject_FunctionExecution* currentExec = _callStack.top();

	const std::vector<ScmObject*>* args = currentExec->getFunctionArgs();
	std::vector<ScmObject*> evaluatedArgs;
	evaluatedArgs.reserve(args->size());

	// Before we actually call our function, we need to make sure our arguments are no function calls.
	int argIndex = 0;

	for (auto it = args->begin(); it != args->end(); ++it, ++argIndex)
	{
		ScmObject* arg = nullptr;

		if (*it == nullptr)
		{
			arg = _result;
			_result = nullptr;
		}
		else
		{
			switch ((*it)->getType())
			{
			case ScmObjectType::FUNCTION_CALL:
			{
				const ScmObject_FunctionCall* funcCall = static_cast<const ScmObject_FunctionCall*>(*it);
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

	const ScmObject* obj = lookupInEnv(currentExec, currentExec->functionCall->getFunctionSymbol());

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
				ScmObject* func = static_cast<const ScmObject_FunctionDefinition*>(obj)->getExecutable(evaluatedArgs, currentExec->environment, currentExec->closureEnvironment);

				if (func->getType() == ScmObjectType::FUNCTION_DEFINITION)
				{
					endExecution(_callStack, _result, func);
				}
				else
				{
					_callStack.pop();
					_callStack.push(static_cast<ScmObject_FunctionExecution*>(func));
				}
			}
			break;
		default:
			return returnError(_callStack, _result, "Tried calling user defined function but symbol supplied is not bound to a user defined function.");
		}
	}
}

void execActual(std::stack<ScmObject_FunctionExecution*>& _callStack, ScmObject*& _result)
{
	ScmObject_FunctionExecution* currentExec = _callStack.top();

	const ScmObject* func = lookupInEnv(currentExec, currentExec->functionCall->getFunctionSymbol());

	if (func->getType() != ScmObjectType::FUNCTION_DEFINITION)
	{
		_result = new ScmObject_InternalError("Tried calling function " + *currentExec->functionCall->getFunctionSymbol()->getName() + " but it has not been defined.");
		while (_callStack.size() > 0)
		{
			_callStack.pop();
		}
		return;
	}

	const ScmObject_FunctionDefinition* funcDef = static_cast<const ScmObject_FunctionDefinition*>(func);

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
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_EQUALS:
		return execBuiltInEquals(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_GT:
		//return execBuiltInGreaterThan(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_LT:
		//return execBuiltInLessThan(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::SYNTAX_DEFINE:
		return execSyntaxDefine(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::SYNTAX_IF:
		return execSyntaxIf(_callStack, _result);
	case ScmObject_FunctionDefinition::FunctionType::CUSTOM:
		return execCustomFunction(_callStack, _result);
	default:
		break;
	}
}

inline ScmObject* execTramp(ScmObject_FunctionExecution* _functionCall)
{
	std::stack<ScmObject_FunctionExecution*> callStack;
	callStack.push(_functionCall);
	ScmObject* result = nullptr;

	while (callStack.size() > 0)
	{
		execActual(callStack, result);
	}

	return result;
}

ScmObject* exec(ScmObject_FunctionExecution* _funcExec)
{
#ifdef MEASURE_EXECUTION_TIME
	auto start = std::chrono::high_resolution_clock::now();
#endif

	ScmObject* result = execTramp(_funcExec);

#ifdef MEASURE_EXECUTION_TIME
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsedTime = end - start;

	std::cout << "Elapsed time for function call: " << (elapsedTime.count() * 1000) << " ms\n";
#endif

	return result;
}