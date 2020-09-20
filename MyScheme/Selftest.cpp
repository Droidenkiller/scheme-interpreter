#include "Selftest.h"
#include "ScmObjectIncludes.h"
#include "BuiltInFunctionsTramp.h"
#include <string>
#include <iostream>

using namespace std;

extern std::shared_ptr<Environment> globalEnvironment;

bool checkFloat(string& _input, double _expectedValue)
{
	bool result = false;
	shared_ptr<ScmObject> object = Reader::ReadNextSymbol(_input);

	if (object != nullptr)
	{
		switch (object->getType())
		{
		case ScmObjectType::FLOAT:
		{
			double value = static_pointer_cast<ScmObject_Float>(object)->getValue();
			if (abs(value - _expectedValue) < numeric_limits<float>::epsilon())
			{
				cout << "Float with expected value of " << _expectedValue << " passed parsing test.\n";
				result = true;
			}
			else
			{
				cerr << "Parsing of float failed. Value did not match expected value."
					" Expected: " << _expectedValue << ", got: " << value << endl;
			}
		}
		break;
		case ScmObjectType::INTERNAL_ERROR:
			cerr << "Internal error on float with value: " << _expectedValue << ". Error: " <<
				static_pointer_cast<ScmObject_InternalError>(object)->getMessage() << endl;
			break;
		default:
			cerr << "Float with expected value of " << _expectedValue << " was parsed to wrong type.\n";
			break;
		}
	}
	else
	{
		cerr << "Got nullptr from parsing. Expected value was: " << _expectedValue << endl;
	}

	return result;
}

bool testFloats()
{
	bool result = true;
	string correctFormats = "-2.33 234.164,.45  , ,,  -32453.2351";
	double numbers[] = { -2.33, 234.164, 0.45, -32453.2351 };

	for (int i = 0; i < 4; ++i)
	{
		if (!checkFloat(correctFormats, numbers[i]))
		{
			result = false;
		}
	}

	return result;
}

bool checkInteger(string& _input, long long _expectedValue)
{
	bool result = false;
	shared_ptr<ScmObject> object = Reader::ReadNextSymbol(_input);

	if (object != nullptr)
	{
		switch (object->getType())
		{
		case ScmObjectType::INT:
		{
			long long value = static_pointer_cast<ScmObject_Integer>(object)->getValue();
			if (value == _expectedValue)
			{
				cout << "Integer with expected value of " << _expectedValue << " passed parsing test.\n";
				result = true;
			}
			else
			{
				cerr << "Parsing of integer failed. Value did not match expected value."
					" Expected: " << _expectedValue << ", got: " << value << endl;
			}
		}
		break;
		case ScmObjectType::INTERNAL_ERROR:
			cerr << "Internal error on integer with value: " << _expectedValue << ". Error: " <<
				static_pointer_cast<ScmObject_InternalError>(object)->getMessage() << endl;
			break;
		default:
			cerr << "Integer with expected value of " << _expectedValue << " was parsed to wrong type.\n";
			break;
		}
	}
	else
	{
		cerr << "Got nullptr from parsing. Expected value was: " << _expectedValue << endl;
	}

	return result;
}

bool testIntegers()
{
	bool result = true;
	string correctFormats = "-233 234164,045  , ,,  -324532351, 0";
	double numbers[] = { -233, 234164, 45, -324532351, 0 };

	for (int i = 0; i < 5; ++i)
	{
		if (!checkInteger(correctFormats, numbers[i]))
		{
			result = false;
		}
	}

	return result;
}

bool checkString(string& _input, string _expectedValue)
{
	bool result = false;
	shared_ptr<ScmObject> object = Reader::ReadNextSymbol(_input);

	if (object != nullptr)
	{
		switch (object->getType())
		{
		case ScmObjectType::STRING:
		{
			string value = *static_pointer_cast<ScmObject_String>(object)->getValue();
			if (value == _expectedValue)
			{
				cout << "String with expected value of \"" << _expectedValue << "\" passed parsing test.\n";
				result = true;
			}
			else
			{
				cerr << "Parsing of string failed. Value did not match expected value."
					" Expected: " << _expectedValue << ", got: " << value << endl;
			}
		}
		break;
		case ScmObjectType::INTERNAL_ERROR:
			cerr << "Internal error on string with value: " << _expectedValue << ". Error: " <<
				static_pointer_cast<ScmObject_InternalError>(object)->getMessage() << endl;
			break;
		default:
			cerr << "String with expected value of " << _expectedValue << " was parsed to wrong type.\n";
			break;
		}
	}
	else
	{
		cerr << "Got nullptr from parsing. Expected value was: " << _expectedValue << endl;
	}

	return result;
}

bool testStrings()
{
	bool result = true;
	string correctFormats = "\"Hello\" \"234164,045\"  , ,,  \"-324532351\", \"0\"";
	string strings[] = { "Hello", "234164,045", "-324532351", "0" };

	for (int i = 0; i < 4; ++i)
	{
		if (!checkString(correctFormats, strings[i]))
		{
			result = false;
		}
	}

	return result;
}

bool checkBool(string& _input, bool _expectedValue)
{
	bool result = false;
	shared_ptr<ScmObject> object = Reader::ReadNextSymbol(_input);
	string expectedValueString = (_expectedValue == true ? "#t" : "#f");

	if (object != nullptr)
	{
		switch (object->getType())
		{
		case ScmObjectType::BOOL:
		{
			const bool value = static_pointer_cast<ScmObject_Bool>(object)->getValue();
			if (value == _expectedValue)
			{
				cout << "Bool with expected value of " << expectedValueString << " passed parsing test.\n";
				result = true;
			}
			else
			{
				cerr << "Parsing of bool failed. Value did not match expected value."
					" Expected: " << expectedValueString << ", got: " << value << endl;
			}
		}
		break;
		case ScmObjectType::INTERNAL_ERROR:
			cerr << "Internal error on bool with value: " << _expectedValue << ". Error: " <<
				static_pointer_cast<ScmObject_InternalError>(object)->getMessage() << endl;
			break;
		default:
			cerr << "Bool with expected value of " << expectedValueString << " was parsed to wrong type.\n";
			break;
		}
	}
	else
	{
		cerr << "Got nullptr from parsing. Expected value was: " << expectedValueString << endl;
	}

	return result;
}

bool testBools()
{
	bool result = true;
	string correctFormats = "#f #t  , ,,  #f,#t, #t";
	bool bools[] = { false, true, false, true, true };

	for (int i = 0; i < 5; ++i)
	{
		if (!checkBool(correctFormats, bools[i]))
		{
			result = false;
		}
	}

	return result;
}

bool testDefine()
{
	string data = "(define a \"Hello\")";
	std::shared_ptr<ScmObject> obj = Reader::ReadNextSymbol(data);

	if (obj->getType() == ScmObjectType::FUNCTION_CALL)
	{
		exec(static_pointer_cast<ScmObject_FunctionCall>(obj)->createFunctionExecution(nullptr, nullptr));
	}
	else
	{
		cout << "Error in define test. \"(define a \"Hello\")\" did not evaluate to a function call." << endl;
		return false;
	}

	data = "a";
	obj = Reader::ReadNextSymbol(data);
	if (obj->getType() == ScmObjectType::SYMBOL)
	{
		shared_ptr<const ScmObject> val = globalEnvironment->getSymbol(static_pointer_cast<ScmObject_Symbol>(obj));

		if (val == nullptr)
		{
			cout << "Symbol " << obj->getOutputString() << " is not defined. Error in define selftest." << endl;
			return false;
		}
		else
		{
			cout << "define test succesful. 'a' defined as " << val->getOutputString() << "." << std::endl;
			return true;
		}
	}
	else
	{
		cout << "Error in define test. 'a' did not evaluate to a symbol." << endl;
		return false;
	}
}

bool testDefineFunction()
{
	string data = "(define (a n) \"Hello\" (if (eq? n 1) 1 (* n (a (- n 1)))))";
	string expectedFunctionDefinition = "(lambda (n) (\"Hello\" (if (eq? n 1) 1 (* n (a (- n 1))))))";
	std::shared_ptr<ScmObject> obj = Reader::ReadNextSymbol(data);

	if (obj->getType() == ScmObjectType::FUNCTION_CALL)
	{
		exec(static_pointer_cast<ScmObject_FunctionCall>(obj)->createFunctionExecution(nullptr, nullptr));
	}
	else
	{
		cout << "Error in define function test. \"(define (a n) \"Hello\" (if (eq? n 1) 1 (* n (a (- n 1)))))\" did not evaluate to a function call." << endl;
		return false;
	}

	data = "a";
	obj = Reader::ReadNextSymbol(data);
	if (obj->getType() == ScmObjectType::SYMBOL)
	{
		shared_ptr<const ScmObject> val = globalEnvironment->getSymbol(static_pointer_cast<ScmObject_Symbol>(obj));

		if (val == nullptr)
		{
			cout << "Symbol " << obj->getOutputString() << " is not defined. Error in define function selftest." << endl;
			return false;
		}
		else
		{
			if (val->getOutputString() == expectedFunctionDefinition)
			{
				cout << "define function test succesful. 'a' defined as " << val->getOutputString() << "." << std::endl;
				return true;
			}
			else
			{
				cout << "Error in define function test. Symbol was defined but did not have the expected value. Expected: " << expectedFunctionDefinition << ", got: " << val->getOutputString() << endl;
			}
		}
	}
	else
	{
		cout << "Error in define function test. 'a' did not evaluate to a symbol." << endl;
		return false;
	}
}

// Tests the lambda expression with set. Define works if the previous test were successful.
bool testLambdaSet()
{
	// Use the factorial function, to cover a wide range of functions and test huge recursion as well.
	string data = "(set! a (lambda (n) \"Hello\" (if (eq? n 1) 1 (* n (a (- n 1))))))";
	string expectedFunctionDefinition = "(lambda (n) (\"Hello\" (if (eq? n 1) 1 (* n (a (- n 1))))))";
	std::shared_ptr<ScmObject> obj = Reader::ReadNextSymbol(data);

	if (obj->getType() == ScmObjectType::FUNCTION_CALL)
	{
		exec(static_pointer_cast<ScmObject_FunctionCall>(obj)->createFunctionExecution(nullptr, nullptr));
	}
	else
	{
		cout << "Error in lambda set test. \"(set! a (lambda (n) \"Hello\" (if (eq? n 1) 1 (* n (a (- n 1))))))\" did not evaluate to a function call." << endl;
		return false;
	}

	data = "a";
	obj = Reader::ReadNextSymbol(data);
	if (obj->getType() == ScmObjectType::SYMBOL)
	{
		shared_ptr<const ScmObject> val = globalEnvironment->getSymbol(static_pointer_cast<ScmObject_Symbol>(obj));

		if (val == nullptr)
		{
			cout << "Symbol " << obj->getOutputString() << " is not defined. Error in lambda set selftest." << endl;
			return false;
		}
		else
		{
			if (val->getOutputString() == expectedFunctionDefinition)
			{
				cout << "Lambda set test succesful. 'a' defined as " << val->getOutputString() << "." << std::endl;
				return true;
			}
			else
			{
				cout << "Error in lambda set test. Symbol was defined but did not have the expected value. Expected: " << expectedFunctionDefinition << ", got: " << val->getOutputString() << endl;
			}
		}
	}
	else
	{
		cout << "Error in lambda set test. 'a' did not evaluate to a symbol." << endl;
		return false;
	}
}

bool testFunctionCallA()
{
	string data = "(a 4)";
	string functionDefinition = "(lambda (n) (\"Hello\" (if (eq? n 1) 1 (* n (a (- n 1))))))";
	std::shared_ptr<ScmObject> obj = Reader::ReadNextSymbol(data);
	std::shared_ptr<ScmObject> result;

	if (obj->getType() == ScmObjectType::FUNCTION_CALL)
	{
		 result = exec(static_pointer_cast<ScmObject_FunctionCall>(obj)->createFunctionExecution(nullptr, nullptr));

		 if (result->getType() != ScmObjectType::INT)
		 {
			 cout << "Error in function call test. Expected to get value of type integer, but got different type." << endl;
			 return false;
		 }
		 else
		 {
			 if (static_pointer_cast<ScmObject_Integer>(result)->getValue() == 24)
			 {
				 cout << "Function call test succesful. Received expected value of 24, when calling (a 4), where a is " << functionDefinition << endl;
				 return true;
			 }
			 else
			 {
				 cout << "Error in function call test. Expected to receive value of 24 but received " << static_pointer_cast<ScmObject_Integer>(result)->getValue() << "." << endl;
			 }
		 }
	}
	else
	{
		cout << "Error in function call test. \"(a 4)\" did not evaluate to a function call." << endl;
		return false;
	}
}

bool testLargeRecursion()
{
	string data = "(a 50000)";
	std::shared_ptr<ScmObject> obj = Reader::ReadNextSymbol(data);
	std::shared_ptr<ScmObject> result;

	if (obj->getType() == ScmObjectType::FUNCTION_CALL)
	{
		cout << "Running large recursion test. This might take a few seconds." << endl;
		result = exec(static_pointer_cast<ScmObject_FunctionCall>(obj)->createFunctionExecution(nullptr, nullptr));

		if (result->getType() != ScmObjectType::INT)
		{
			cout << "Error in function call test. Expected to get value of type integer, but got different type." << endl;
			return false;
		}
		else
		{
			// The value is nonsense anyways, because we don't have big integers. Just the fact that the method ran to its end is enough to verify large recursion working.
			cout << "Large recursion test ran without crashing. ~50000 recursive calls." << endl;
			return true;
		}
	}
	else
	{
		cout << "Error in large recursion test. \"(a 50000)\" did not evaluate to a function call." << endl;
		return false;
	}
}

bool testScheme()
{
	bool result = true;

	if (!testFloats())
	{
		result = false;
	}
	if (!testIntegers())
	{
		result = false;
	}
	if (!testStrings())
	{
		result = false;
	}
	if (!testBools())
	{
		result = false;
	}
	if (!testDefineFunction())
	{
		result = false;
	}
	if (!testFunctionCallA())
	{
		result = false;
	}
	// Test define needs to be here, so that test lambda set actually has to set the value.
	if (!testDefine())
	{
		result = false;
	}
	if (!testLambdaSet())
	{
		result = false;
	}
	// We use the same function for the lambda define test as for the function define test, so we can reuse testFunctionCallA().
	if (!testFunctionCallA())
	{
		result = false;
	}
	// TODO: Reenable this, to test large recursion. Disabled to lower startup time.
	//if (!testLargeRecursion())
	//{
	//	result = false;
	//}

	return result;
}