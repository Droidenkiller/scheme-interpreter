#include "Reader.h"
#include "ScmObjectIncludes.h"
#include <vector>
#include <iostream>
using namespace std;

ScmObject* readString(string& _remainingInput)
{
	string input = _remainingInput;
	ScmObject* result = nullptr;
	int i = 1;

	for (; i < input.length(); ++i)
	{
		if (input[i] == '"')
		{
			result = new ScmObject_String(input.substr(1, i - 1));
			break;
		}
	}

	_remainingInput = input.substr(i + 1);
	return result;
}

ScmObject* readNumber(string& _remainingInput)
{
	ScmObject* result = nullptr;
	string input = _remainingInput;
	string foundNumber = "";
	bool endOfNumberReached = false;
	bool isFloatingPoint = false;
	int i = 0;

	for (; i < input.length() && !endOfNumberReached; ++i)
	{
		switch (input[i])
		{
		case '-':
			if (foundNumber.length() == 0)
			{
				foundNumber += '-';
			}
			else
			{
				if (result == nullptr)
				{
					result = new ScmObject_InternalError("\"-\" found in the middle of a number. This is invalid.");
				}
			}
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			foundNumber += input[i];
			break;
		case '.':
			if (!isFloatingPoint)
			{
				foundNumber += '.';
				isFloatingPoint = true;
			}
			else
			{
				if (result == nullptr)
				{
					result = new ScmObject_InternalError("Multiple dots found in number. This is invalid.");
				}
			}
			break;
		case ' ':
		case ',':
		case ')':
			endOfNumberReached = true;
			break;
		default:
			break;
		}
	}

	if (result == nullptr)
	{
		if (isFloatingPoint)
		{
			result = new ScmObject_Float(foundNumber);
		}
		else
		{
			result = new ScmObject_Integer(foundNumber);
		}
	}

	_remainingInput = endOfNumberReached ? _remainingInput.substr(i - 1) : _remainingInput.substr(i);

	return result;
}

ScmObject* readBool(string& _remainingInput)
{
#ifdef DEBUG
	if (_remainingInput[0] != '#')
	{
		throw new exception("Tried to read bool but string did not start with '#'. This may never happen and indicates an error in code.");
	}
#endif

	_remainingInput = _remainingInput.substr(1);

	if (_remainingInput.length() > 1 && !(_remainingInput[1] == ' ' || _remainingInput[1] == ',' || _remainingInput[1] == ')'))
	{
		return new ScmObject_InternalError("Tried reading bool but got more characters than expected. Please only use #t or #f.");
	}

	switch (_remainingInput[0])
	{
	case 't':
	case 'T':
		_remainingInput = _remainingInput.substr(1);
		return new ScmObject_Bool(true);
	case 'f':
	case 'F':
		_remainingInput = _remainingInput.substr(1);
		return new ScmObject_Bool(false);
	default:
		return new ScmObject_InternalError("Expected bool symbol of either 't' or 'f' but got " + std::string(1, _remainingInput[0]) + " instead.");
		break;
	}
}

ScmObject_Symbol* readSymbol(string& _remainingInput)
{
	string input = _remainingInput;
	bool endOfSymbol = false;
	int i = 0;

	for (; i < input.length() && !endOfSymbol; ++i)
	{
		switch (input[i])
		{
		case ' ':
		case ',':
		case ')':
			endOfSymbol = true;
			break;
		}
	}

	ScmObject_Symbol* result = endOfSymbol ? new ScmObject_Symbol(input.substr(0, i - 1)) : new ScmObject_Symbol(input.substr(0, i));
	_remainingInput = endOfSymbol ? input.substr(i - 1) : input.substr(i);

	return result;
}

// Skips whitespace and commas.
void skipWhitespace(string& _remainingInput)
{
	for (; 0 < _remainingInput.length();)
	{
		if (_remainingInput[0] == ' ' || _remainingInput[0] == ',')
		{
			_remainingInput = _remainingInput.substr(1);
		}
		else
		{
			return;
		}
	}

	_remainingInput = "";
	return;
}

ScmObject* Reader::readLambda(string& _remainingInput)
{
	// First read the parameter list.
	skipWhitespace(_remainingInput);

	if (_remainingInput[0] != '(')
	{
		_remainingInput = "";
		return new ScmObject_InternalError("Tried to read parameter list of lambda expression but no parameter list was found.");
	}

	// Remove '('
	_remainingInput = _remainingInput.substr(1);

	std::vector<ScmObject_Symbol*> params{};

	while (_remainingInput.length() > 0 && _remainingInput[0] != ')')
	{
		skipWhitespace(_remainingInput);

		// Not enough input.
		if (_remainingInput.length() == 0)
		{
			return nullptr;
		}
		// No parameters supplied -> function without parameters.
		else if (_remainingInput[0] == ')')
		{
			break;
		}

		ScmObject* param = ReadNextSymbol(_remainingInput);

		switch (param->getType())
		{
		case ScmObjectType::SYMBOL:
			params.push_back(static_cast<ScmObject_Symbol*>(param));
			break;
		default:
			_remainingInput = "";
			return new ScmObject_InternalError("Received non-symbol while reading parameter list of lambda expression. This is invalid.");
			break;
		}
	}

	// Remove ')'
	skipWhitespace(_remainingInput);
	if (_remainingInput.length() == 0)
	{
		// There is not enough data in this line.
		return nullptr;
	}
	// The current symbol is definitely ')' due to the while loop above.
	else
	{
		_remainingInput = _remainingInput.substr(1);
	}

	// Then read the function body.
	skipWhitespace(_remainingInput);

	ScmObject* innerFunction = readFunctionKeyword(_remainingInput);

	switch (innerFunction->getType())
	{
	case ScmObjectType::FUNCTION_CALL:
	case ScmObjectType::FUNCTION_DEFINITION:
		break;
	default:
		return new ScmObject_InternalError("Reading function body failed.");
		break;
	}

	skipWhitespace(_remainingInput);

	// The closing paranthesis is missing. We need more input.
	if (_remainingInput.size() == 0)
	{
		return nullptr;
	}
	else if (_remainingInput[0] != ')')
	{
		return new ScmObject_InternalError("Expected ')' at the end of lambda statement. But '" + std::string(1, _remainingInput[0]) + "' was found.");
	}
	else
	{
		// Remove the closing bracket.
		_remainingInput = _remainingInput.substr(1);

		// Finally, create the function definition.
		if (innerFunction->getType() == ScmObjectType::FUNCTION_CALL)
		{
			return new ScmObject_FunctionDefinition(params, static_cast<ScmObject_FunctionCall*>(innerFunction));
		}
		else if (innerFunction->getType() == ScmObjectType::FUNCTION_DEFINITION)
		{
			return new ScmObject_FunctionDefinition(params, static_cast<ScmObject_FunctionDefinition*>(innerFunction));
		}
		else
		{
			return new ScmObject_InternalError("Expected to get lambda or function in function body, but got neither.");
		}
	}
}

ScmObject* Reader::readFunctionKeyword(string& _remainingInput)
{
	int i = 0;

	// If there are no symbols in brackets, we need more input.
	if (_remainingInput.length() < 2)
	{
		_remainingInput = _remainingInput.substr(1);
		return nullptr;
	}

	// Skip '('
	_remainingInput = _remainingInput.substr(1);
	skipWhitespace(_remainingInput);

	if (_remainingInput.length() == 0)
	{
		return nullptr;
	}
	switch (_remainingInput[0])
	{
	case '"':
		return new ScmObject_InternalError("Found \" when expecting a function symbol. This is invalid.");
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return new ScmObject_InternalError("Found number when expecting a function symbol. This is invalid.");
	case '.':
		return new ScmObject_InternalError("Found . when expecting a function symbol. This is invalid.");
	}

	ScmObject_Symbol* functionSymbol = readSymbol(_remainingInput);

	if (functionSymbol->getName()->compare("lambda") == 0)
	{
		return readLambda(_remainingInput);
	}
	else
	{
		vector<ScmObject*> args = vector<ScmObject*>();

		while (_remainingInput.length() > 0 && _remainingInput[0] != ')')
		{
			skipWhitespace(_remainingInput);

			// When there is no further data, we need more input.
			if (_remainingInput.length() == 0)
			{
				return nullptr;
			}
			else if (_remainingInput[0] == ')')
			{
				break;
			}

			args.push_back(ReadNextSymbol(_remainingInput));
		}

		// When there is no further data or we do not end with a ')', we need more input.
		if (_remainingInput.length() == 0 || _remainingInput[0] != ')')
		{
			return nullptr;
		}
		else
		{
			// Remove the closing paranthesis
			_remainingInput = _remainingInput.substr(1);

			return new ScmObject_FunctionCall(functionSymbol, args);
		}
	}
}

ScmObject* Reader::ReadNextSymbol(string& _remainingInput)
{
	string input = _remainingInput;
	ScmObject* result = nullptr;
	bool foundObject = false;
	int i = 0;

	for (; i < input.length() && !foundObject; ++i)
	{
		switch (input[i])
		{
		case ' ':
		case ',':
			break;
		case '"':
			_remainingInput = input.substr(i);
			result = readString(_remainingInput);
			foundObject = true;
			break;
		case '(':
			_remainingInput = input.substr(i);
			result = readFunctionKeyword(_remainingInput);
			foundObject = true;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
		case '-':
			_remainingInput = input.substr(i);
			result = readNumber(_remainingInput);
			foundObject = true;
			break;
		case '#':
			_remainingInput = input.substr(i);
			result = readBool(_remainingInput);
			foundObject = true;
			break;
		case ')':
			_remainingInput = "";
			result = new ScmObject_InternalError("Found ')' without opening brackets. This is invalid.");
			foundObject = true;
			break;
		default:
			_remainingInput = input.substr(i);
			result = readSymbol(_remainingInput);
			foundObject = true;
		}
	}

	return result;
}