#include "ScmObject_FunctionDefinition.h"
#include "ScmObject_InternalError.h"
#include <string>

std::unordered_map<ScmObject_FunctionDefinition::FunctionType, std::shared_ptr<ScmObject_FunctionDefinition>> ScmObject_FunctionDefinition::m_builtInFunctions;

ScmObject_FunctionDefinition::ScmObject_FunctionDefinition(ScmObject_FunctionDefinition::FunctionType _functionType) : ScmObject(ScmObjectType::FUNCTION_DEFINITION)
{
	m_functionType = _functionType;
	m_infiniteParams = true;
}

ScmObject_FunctionDefinition::ScmObject_FunctionDefinition(std::vector<std::shared_ptr<ScmObject_Symbol>> _params, std::vector<std::shared_ptr<ScmObject>>& _innerFunctions, std::shared_ptr<Environment> _parentEnv, std::shared_ptr<Environment> _closureEnv) : ScmObject(ScmObjectType::FUNCTION_DEFINITION)
{
	m_functionType = ScmObject_FunctionDefinition::FunctionType::CUSTOM;

	m_infiniteParams = false;
	m_params = _params;
	m_innerFunctions = _innerFunctions;
	if (_parentEnv != nullptr)
	{
		m_functionDefinitionEnvironment = std::make_shared<Environment>(*_parentEnv);
	}
	else
	{
		m_functionDefinitionEnvironment = std::make_shared<Environment>();
	}
	// Merge parent env with closure env
	if (_closureEnv != nullptr)
	{
		m_functionDefinitionEnvironment->setParent(std::make_shared<Environment>(*_closureEnv));
	}
}

//ScmObject_FunctionDefinition::ScmObject_FunctionDefinition(ScmObject_FunctionCall* _innerFunction) : ScmObject(ScmObjectType::FUNCTION_DEFINITION)
//{
//	m_functionType = ScmObject_FunctionDefinition::FunctionType::CUSTOM;
//
//	m_infiniteParams = true;
//	m_innerFunction = _innerFunction;
//}

ScmObject_FunctionDefinition::FunctionType ScmObject_FunctionDefinition::getFunctionType() const
{
	return m_functionType;
}

std::vector<std::shared_ptr<ScmObject>> ScmObject_FunctionDefinition::getExecutable(const std::vector<std::shared_ptr<ScmObject>>& _args) const
{
	if (m_innerFunctions[m_innerFunctions.size() - 1]->getType() == ScmObjectType::FUNCTION_CALL)
	{
		if (std::static_pointer_cast<ScmObject_FunctionCall>(m_innerFunctions[m_innerFunctions.size() - 1])->getFunctionSymbol()->getName()->compare("define") == 0)
		{
			return std::vector<std::shared_ptr<ScmObject>>{std::make_shared<ScmObject_InternalError>("Last function call of user defined function was a call to 'define'. This is invalid. Problematic function was: " + getOutputString())};
		}
	}

	if (_args.size() != m_params.size() && !m_infiniteParams)
	{
		return std::vector<std::shared_ptr<ScmObject>>{std::make_shared<ScmObject_InternalError>("Argument count did not match parameter count. Expected " + std::to_string(m_params.size()) + " got " + std::to_string(_args.size()) + ".")};
	}

	// Create the new closure environment
	std::shared_ptr<Environment> argsEnv = std::make_shared<Environment>();
	auto param = m_params.begin();
	auto arg = _args.begin();
	for (; arg != _args.end() && param != m_params.end(); ++arg, ++param)
	{
		argsEnv->addSymbol(*param, *arg);
	}

	std::vector<std::shared_ptr<ScmObject>> execs;
	execs.reserve(m_innerFunctions.size());

	for (auto func : m_innerFunctions)
	{
		switch (func->getType())
		{
		case ScmObjectType::FUNCTION_CALL:
			{
				std::shared_ptr<ScmObject_FunctionCall> call = std::static_pointer_cast<ScmObject_FunctionCall>(func);
				execs.push_back(call->createFunctionExecution(argsEnv, m_functionDefinitionEnvironment));
			}
			break;
		default:
			execs.push_back(func);
			break;
		}
	}

	return execs;
}

bool ScmObject_FunctionDefinition::equals(const ScmObject* _other) const
{
	return this == _other;
}

std::shared_ptr<const ScmObject> ScmObject_FunctionDefinition::copy() const
{
	return std::make_shared<ScmObject_FunctionDefinition>(*this);
}

const std::string ScmObject_FunctionDefinition::getOutputString() const
{
	switch (m_functionType)
	{
	case FunctionType::BUILT_IN_ADD:
		return "<Function:+>";
	case FunctionType::BUILT_IN_SUBTRACT:
		return "<Function:->";
	case FunctionType::BUILT_IN_MULTIPLY:
		return "<Function:*>";
	case FunctionType::BUILT_IN_DIVIDE:
		return "<Function:/>";
	case FunctionType::BUILT_IN_EQUALS:
		return "<Function:eq?>";
	case FunctionType::BUILT_IN_GT:
		return "<Function:>>";
	case FunctionType::BUILT_IN_LT:
		return "<Function:<>";
	case FunctionType::BUILT_IN_CONS:
		return "<Function:cons>";
	case FunctionType::BUILT_IN_CAR:
		return "<Function:car>";
	case FunctionType::BUILT_IN_CDR:
		return "<Function:cdr>";
	case FunctionType::BUILT_IN_STRINGQ:
		return "<Function:string?>";
	case FunctionType::BUILT_IN_CONSQ:
		return "<Function:cons?>";
	case FunctionType::BUILT_IN_NUMBERQ:
		return "<Function:number?>";
	case FunctionType::BUILT_IN_FUNCTIONQ:
		return "<Function:function?>";
	case FunctionType::BUILT_IN_USER_DEFINED_FUNCTIONQ:
		return "<Function:udf?>";
	case FunctionType::BUILT_IN_PRINT:
		return "<Function:print>";
	case FunctionType::BUILT_IN_DISPLAY:
		return "<Function:display>";
	case FunctionType::BUILT_IN_FUNCTION_BODY:
		return "<Function:function-body>";
	case FunctionType::BUILT_IN_FUNCTION_ARG_LIST:
		return "<Function:function-arglist>";
	case FunctionType::BUILT_IN_READ_LINE:
		return "<Function:read-line>";
	case FunctionType::BUILT_IN_LOAD:
		return "<Function:load>";
	case FunctionType::SYNTAX_QUOTE:
		return "<Syntax:quote>";
	case FunctionType::SYNTAX_LAMBDA:
		return "<Syntax:lambda>";
	case FunctionType::SYNTAX_DEFINE:
		return "<Syntax:define>";
	case FunctionType::SYNTAX_IF:
		return "<Syntax:if>";
	case FunctionType::SYNTAX_SET:
		return "<Syntax:set!>";
	case FunctionType::SYNTAX_BEGIN:
		return "<Syntax:begin>";
	default:
		{
			std::string output = "(lambda (";

			bool initialRun = true;
			for (auto param : m_params)
			{
				if (initialRun)
				{
					initialRun = false;
				}
				else
				{
					output += " ";
				}
				
				output += *param->getName();
			}

			output += ") (";

			for (auto it = m_innerFunctions.begin(); it != m_innerFunctions.end(); )
			{
				output += (*it)->getOutputString();
				++it;
				if (it != m_innerFunctions.end())
				{
					output += " ";
				}
			}

			output += "))";

			return output;
		}
		break;
	}
}

const std::string ScmObject_FunctionDefinition::getDisplayString() const
{
	return getOutputString();
}

const std::shared_ptr<ScmObject_Cons> ScmObject_FunctionDefinition::getFunctionBody() const
{
	std::shared_ptr<ScmObject_Cons> result = nullptr;

	for (auto it = m_innerFunctions.rbegin(); it != m_innerFunctions.rend(); ++it)
	{
		if ((*it)->getType() == ScmObjectType::FUNCTION_CALL)
		{
			result = std::make_shared<ScmObject_Cons>(std::static_pointer_cast<ScmObject_FunctionCall>(*it)->makeCons(), result);
		}
		else
		{
			result = std::make_shared<ScmObject_Cons>(*it, result);
		}
	}

	return result;
}

const std::shared_ptr<ScmObject_Cons> ScmObject_FunctionDefinition::getFunctionArgList() const
{
	std::shared_ptr<ScmObject_Cons> result = nullptr;
	for (auto it = m_params.rbegin(); it != m_params.rend(); ++it)
	{
		result = std::make_shared<ScmObject_Cons>(*it, result);
	}
	return result;
}

// This is deprecated and not needed anymore.
//const std::shared_ptr<ScmObject_Cons> ScmObject_FunctionDefinition::makeCons() const
//{
//	auto result = std::make_shared<ScmObject_Cons>(getFunctionArgList(), nullptr);
//	/*result = std::make_shared<ScmObject_Cons>(m_innerFunctionDefinition->getFunctionArgList(), result);
//	result = std::make_shared<ScmObject_Cons>(std::make_shared<ScmObject_Symbol>("lambda"), result);*/
//
//	return result;
//}