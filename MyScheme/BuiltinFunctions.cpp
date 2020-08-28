#ifdef OLD_BUILT_INS

#include "BuiltinFunctions.h"
#ifdef DEBUG
#include <exception>
#endif
#include <iostream>
#include <string>

const ScmObject* lookupInEnv(const Environment* _env, const Environment* _closureEnv, const ScmObject_Symbol* _symbol)
{
	const ScmObject* boundSymbol = nullptr;
	if (_closureEnv != nullptr)
	{
		boundSymbol = _closureEnv->getSymbol(_symbol);
	}
	if (boundSymbol == nullptr)
	{
		boundSymbol = _env->getSymbol(_symbol);
	}

	return boundSymbol;
}

bool tryExecBuiltIn(const ScmObject_FunctionCall* _functionCall, Environment* _env, Environment* _closureEnv, ScmObject*& _out)
{
	const ScmObject* boundSymbol = lookupInEnv(_env, _closureEnv, _functionCall->getFunctionSymbol());

	if (boundSymbol == nullptr)
	{
		_out = nullptr;
		return false;
	}
	if (boundSymbol->getType() != ScmObjectType::FUNCTION_DEFINITION)
	{
		_out = new ScmObject_InternalError(*_functionCall->getFunctionSymbol()->getName() + " was not bound to a function.");
		return true;
	}

	const ScmObject_FunctionDefinition* funcDef = static_cast<const ScmObject_FunctionDefinition*>(boundSymbol);

	switch (funcDef->getFunctionType())
	{
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_ADD:
		{
			_out = builtInAdd(_functionCall, _env, _closureEnv);
			return true;
		}
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_SUBTRACT:
		{
			_out = builtInSubtract(_functionCall);
			return true;
		}
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_MULTIPLY:
		{
			_out = builtInMultiply(_functionCall);
			return true;
		}
	case ScmObject_FunctionDefinition::FunctionType::BUILT_IN_DIVIDE:
		{
			_out = builtInDivide(_functionCall);
			return true;
		}
	case ScmObject_FunctionDefinition::FunctionType::SYNTAX_DEFINE:
		{
			_out = syntaxDefine(_functionCall, _env, _closureEnv);
			return true;
		}
	case ScmObject_FunctionDefinition::FunctionType::SYNTAX_IF:
		{
			_out = syntaxIf(_functionCall, _env, _closureEnv);
			return true;
		}
	// TODO: Add other built-in functions here.
	default:
		{
			_out = nullptr;
			return false;
		}
	}
}

ScmObject* builtInAdd(const ScmObject_FunctionCall* _functionCall, Environment* _env, Environment* _closureEnv)
{
#ifdef DEBUG
	if (_functionCall->getFunctionSymbol()->getName()->compare(OPERATOR_ADD) != 0)
	{
		throw new std::exception("builtInAdd() was executed even though the function was not the add function. This should never happen and implies an error in code.");
	}
#endif

	const std::vector<ScmObject*> args = *(_functionCall->getFunctionArgs());

	if (args.size() == 0)
	{
		return new ScmObject_InternalError("Built-in function add was called without any arguments.");
	}

	union {
		double floatSum;
		long long integerSum;
	} sum = { 0 };

	bool useFloatSum = false;
	
	for (auto arg : args)
	{
		switch (arg->getType())
		{
		case ScmObjectType::FLOAT:
			{
				double val = static_cast<ScmObject_Float*>(arg)->getValue();

				if (useFloatSum)
				{
					sum.floatSum += val;
				}
				else
				{
					useFloatSum = true;
					sum.floatSum = sum.integerSum + val;
				}
			}
			break;
		case  ScmObjectType::INT:
			{
				long long val = static_cast<ScmObject_Integer*>(arg)->getValue();

				if (useFloatSum)
				{
					sum.floatSum += val;
				}
				else
				{
					sum.integerSum += val;
				}
			}
			break;
		case ScmObjectType::SYMBOL:
			{
				const ScmObject* obj = nullptr;
				if (_closureEnv != nullptr)
				{
					obj = _closureEnv->getSymbol(static_cast<ScmObject_Symbol*>(arg));
				}
				if (obj == nullptr)
				{
					obj = _env->getSymbol(static_cast<ScmObject_Symbol*>(arg));
				}

				switch (obj->getType())
				{
				case ScmObjectType::FLOAT:
				{
					double val = static_cast<const ScmObject_Float*>(obj)->getValue();

					if (useFloatSum)
					{
						sum.floatSum += val;
					}
					else
					{
						useFloatSum = true;
						sum.floatSum = sum.integerSum + val;
					}
				}
				break;
				case  ScmObjectType::INT:
				{
					long long val = static_cast<const ScmObject_Integer*>(obj)->getValue();

					if (useFloatSum)
					{
						sum.floatSum += val;
					}
					else
					{
						sum.integerSum += val;
					}
				}
				break;
				default:
					break;
				}
			}
			break;
		case ScmObjectType::FUNCTION_CALL:
			{
				// TODO: Execute function and do same as above.
				// TODO: Add tail call elimination
			}
			break;
		default:
			return new ScmObject_InternalError("Expected value of type float or integer in built-in add function but received value of different type.");
			break;
		}
	}

	if (useFloatSum)
	{
		return new ScmObject_Float(sum.floatSum);
	}
	else
	{
		return new ScmObject_Integer(sum.integerSum);
	}
}

ScmObject* builtInSubtract(const ScmObject_FunctionCall* _functionCall)
{
#ifdef DEBUG
	if (_functionCall->getFunctionSymbol()->getName()->compare(OPERATOR_SUBTRACT) != 0)
	{
		throw new std::exception("builtInSubtract() was executed even though the function was not the subtract function. This should never happen and implies an error in code.");
	}
#endif

	const std::vector<ScmObject*> args = *(_functionCall->getFunctionArgs());

	if (args.size() == 0)
	{
		return new ScmObject_InternalError("Built-in function subtract was called without any arguments.");
	}
	// Negate the argument if there is only one argument.
	else if (args.size() == 1)
	{
		switch (args[0]->getType())
		{
		case ScmObjectType::FLOAT:
			{
				return new ScmObject_Float(-static_cast<ScmObject_Float*>(args[0])->getValue());
			}
			break;
		case ScmObjectType::INT:
			{
				return new ScmObject_Integer(-static_cast<ScmObject_Integer*>(args[0])->getValue());
			}
			break;
		case ScmObjectType::SYMBOL:
			{
				// TODO: Get symbol value from environment and do same as above.
			}
			break;
		case ScmObjectType::FUNCTION_CALL:
			{
				// TODO: Execute function and do same as above.
				// TODO: Add tail call elimination
			}
			break;
		default:
			return new ScmObject_InternalError("Expected value of type float or integer in built-in subtract function but received value of different type.");
			break;
		}
	}

	union {
		double floatSum;
		long long integerSum;
	} sum = { 0 };

	bool useFloatSum = false;
	bool firstRun = true;

	for (auto arg : args)
	{
		switch (arg->getType())
		{
		case ScmObjectType::FLOAT:
		{
			double val = static_cast<ScmObject_Float*>(arg)->getValue();

			if (useFloatSum)
			{
				if (firstRun)
				{
					sum.floatSum = val;
				}
				else
				{
					sum.floatSum -= val;
				}
			}
			else
			{
				useFloatSum = true;

				if (firstRun)
				{
					sum.floatSum = val;
				}
				else
				{
					sum.floatSum = sum.integerSum - val;
				}
			}
		}
		break;
		case  ScmObjectType::INT:
		{
			long long val = static_cast<ScmObject_Integer*>(arg)->getValue();

			if (useFloatSum)
			{
				if (firstRun)
				{
					sum.floatSum = static_cast<double>(val);
				}
				else
				{
					sum.floatSum -= val;
				}
			}
			else
			{
				if (firstRun)
				{
					sum.integerSum = val;
				}
				else
				{
					sum.integerSum -= val;
				}
			}
		}
		break;
		case ScmObjectType::SYMBOL:
		{
			// TODO: Get symbol value from environment and do same as above.
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			// TODO: Execute function and do same as above.
			// TODO: Add tail call elimination
		}
		break;
		default:
			return new ScmObject_InternalError("Expected value of type float or integer in built-in subtract function but received value of different type.");
			break;
		}

		firstRun = false;
	}

	if (useFloatSum)
	{
		return new ScmObject_Float(sum.floatSum);
	}
	else
	{
		return new ScmObject_Integer(sum.integerSum);
	}
}

ScmObject* builtInMultiply(const ScmObject_FunctionCall* _functionCall)
{
#ifdef DEBUG
	if (_functionCall->getFunctionSymbol()->getName()->compare(OPERATOR_DIVIDE) != 0)
	{
		throw new std::exception("builtInMultiply() was executed even though the function was not the multiply function. This should never happen and implies an error in code.");
	}
#endif

	const std::vector<ScmObject*> args = *(_functionCall->getFunctionArgs());

	if (args.size() == 0)
	{
		return new ScmObject_InternalError("Built-in function multiply was called without any arguments.");
	}
	// Our result is the argument itself if there is only one argument.
	else if (args.size() == 1)
	{
		switch (args[0]->getType())
		{
		case ScmObjectType::FLOAT:
		{
			return new ScmObject_Float(static_cast<ScmObject_Float*>(args[0])->getValue());
		}
		break;
		case ScmObjectType::INT:
		{
			return new ScmObject_Integer(static_cast<ScmObject_Integer*>(args[0])->getValue());
		}
		break;
		case ScmObjectType::SYMBOL:
		{
			// TODO: Get symbol value from environment and do same as above.
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			// TODO: Execute function and do same as above.
			// TODO: Add tail call elimination
		}
		break;
		default:
			return new ScmObject_InternalError("Expected value of type float or integer in built-in multiply function but received value of different type.");
			break;
		}
	}

	union {
		double floatSum;
		long long integerSum;
	} sum = { 0 };

	bool useFloatSum = false;
	bool firstRun = true;

	for (auto arg : args)
	{
		switch (arg->getType())
		{
		case ScmObjectType::FLOAT:
		{
			double val = static_cast<ScmObject_Float*>(arg)->getValue();

			if (useFloatSum)
			{
				if (firstRun)
				{
					sum.floatSum = val;
				}
				else
				{
					sum.floatSum *= val;
				}
			}
			else
			{
				useFloatSum = true;

				if (firstRun)
				{
					sum.floatSum = val;
				}
				else
				{
					sum.floatSum = sum.integerSum * val;
				}
			}
		}
		break;
		case  ScmObjectType::INT:
		{
			long long val = static_cast<ScmObject_Integer*>(arg)->getValue();

			if (useFloatSum)
			{
				if (firstRun)
				{
					sum.floatSum = static_cast<double>(val);
				}
				else
				{
					sum.floatSum *= val;
				}
			}
			else
			{
				if (firstRun)
				{
					sum.integerSum = val;
				}
				else
				{
					sum.integerSum *= val;
				}
			}
		}
		break;
		case ScmObjectType::SYMBOL:
		{
			// TODO: Get symbol value from environment and do same as above.
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			// TODO: Execute function and do same as above.
			// TODO: Add tail call elimination
		}
		break;
		default:
			return new ScmObject_InternalError("Expected value of type float or integer in built-in multiply function but received value of different type.");
			break;
		}

		firstRun = false;
	}

	if (useFloatSum)
	{
		return new ScmObject_Float(sum.floatSum);
	}
	else
	{
		return new ScmObject_Integer(sum.integerSum);
	}
}

ScmObject* builtInDivide(const ScmObject_FunctionCall* _functionCall)
{
#ifdef DEBUG
	if (_functionCall->getFunctionSymbol()->getName()->compare(OPERATOR_DIVIDE) != 0)
	{
		throw new std::exception("builtInDivide() was executed even though the function was not the divide function. This should never happen and implies an error in code.");
	}
#endif

	const std::vector<ScmObject*> args = *(_functionCall->getFunctionArgs());

	if (args.size() == 0)
	{
		return new ScmObject_InternalError("Built-in function divide was called without any arguments.");
	}
	if (args.size() == 1)
	{
		switch (args[0]->getType())
		{
		case ScmObjectType::FLOAT:
		{
			double val = static_cast<ScmObject_Float*>(args[0])->getValue();
			return new ScmObject_Float(1 / val);
		}
		break;
		case  ScmObjectType::INT:
		{
			long long val = static_cast<ScmObject_Integer*>(args[0])->getValue();
			return new ScmObject_Float(1.0 / val);
		}
		break;
		case ScmObjectType::SYMBOL:
		{
			// TODO: Get symbol value from environment and do same as above.
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			// TODO: Execute function and do same as above.
			// TODO: Add tail call elimination
		}
		break;
		default:
			return new ScmObject_InternalError("Expected value of type float or integer in built-in divide function but received value of different type.");
			break;
		}
	}

	double floatSum = 0;
	bool firstRun = true;

	for (auto arg : args)
	{
		switch (arg->getType())
		{
		case ScmObjectType::FLOAT:
		{
			double val = static_cast<ScmObject_Float*>(arg)->getValue();

			if (firstRun)
			{
				floatSum = val;
			}
			else
			{
				floatSum /= val;
			}
		}
		break;
		case  ScmObjectType::INT:
		{
			long long val = static_cast<ScmObject_Integer*>(arg)->getValue();

			if (firstRun)
			{
				floatSum = static_cast<double>(val);
			}
			else
			{
				floatSum /= val;
			}
		}
		break;
		case ScmObjectType::SYMBOL:
		{
			// TODO: Get symbol value from environment and do same as above.
		}
		break;
		case ScmObjectType::FUNCTION_CALL:
		{
			// TODO: Execute function and do same as above.
			// TODO: Add tail call elimination
		}
		break;
		default:
			return new ScmObject_InternalError("Expected value of type float or integer in built-in divide function but received value of different type.");
			break;
		}

		firstRun = false;
	}

	return new ScmObject_Float(floatSum);
}

ScmObject* syntaxDefine(const ScmObject_FunctionCall* _functionCall, Environment* _env, Environment* _closureEnv)
{
	const std::vector<ScmObject*>* args = _functionCall->getFunctionArgs();

	if (args->size() != 2)
	{
		return new ScmObject_InternalError("Called define with argument count higher or lower than 2. This is not allowed.");
	}

	if ((*args)[0]->getType() != ScmObjectType::SYMBOL)
	{
		return new ScmObject_InternalError("First argument was no symbol when calling define.");
	}

	// If the second argument is a function call, we first need to execute the function.
	if ((*args)[1]->getType() == ScmObjectType::FUNCTION_CALL)
	{
		_env->addSymbol(static_cast<ScmObject_Symbol*>((*args)[0]), static_cast<ScmObject_FunctionCall*>((*args)[1])->Execute(_env, _closureEnv));
	}
	// If the second argument is a symbol we first need to look it up in the environment.
	else if ((*args)[1]->getType() == ScmObjectType::SYMBOL)
	{
		const ScmObject* obj = lookupInEnv(_env, _closureEnv, static_cast<ScmObject_Symbol*>((*args)[1]));

		if (obj == nullptr)
		{
			return new ScmObject_InternalError("Symbol " + (*args)[1]->getOutputString() + " is not defined.");
		}
		else
		{
			_env->addSymbol(static_cast<ScmObject_Symbol*>((*args)[0]), obj);
		}
	}
	// If the second argument is an error, we return this error.
	else if ((*args)[1]->getType() == ScmObjectType::INTERNAL_ERROR)
	{
		return (*args)[1];
	}
	else
	{
		_env->addSymbol(static_cast<ScmObject_Symbol*>((*args)[0]), (*args)[1]);
	}

	return (*args)[0];
}

ScmObject* syntaxIf(const ScmObject_FunctionCall* _functionCall, Environment* _env, Environment* _closureEnv)
{
	const std::vector<ScmObject*>* args = _functionCall->getFunctionArgs();

	if (args->size() != 3)
	{
		return new ScmObject_InternalError("'if' expected 3 arguments but got " + std::to_string(args->size()));
	}

	switch ((*args)[0]->getType())
	{
	case ScmObjectType::BOOL:
		{
			bool val = static_cast<ScmObject_Bool*>((*args)[0])->getValue();

			ScmObject* nextExpr;

			if (val == true)
			{
				nextExpr = (*args)[1];
			}
			else
			{
				nextExpr = (*args)[2];
			}

			switch (nextExpr->getType())
			{
			case ScmObjectType::FUNCTION_CALL:
				return static_cast<ScmObject_FunctionCall*>(nextExpr)->Execute(_env, _closureEnv);
			default:
				return nextExpr;
			}
		}
	}
}

#endif