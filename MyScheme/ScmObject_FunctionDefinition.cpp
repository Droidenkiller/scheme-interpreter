#include "ScmObject_FunctionDefinition.h"
#include "ScmObject_InternalError.h"
#include <string>

std::unordered_map<ScmObject_FunctionDefinition::FunctionType, std::shared_ptr<ScmObject_FunctionDefinition>> ScmObject_FunctionDefinition::m_builtInFunctions;

ScmObject_FunctionDefinition::ScmObject_FunctionDefinition(ScmObject_FunctionDefinition::FunctionType _functionType) : ScmObject(ScmObjectType::FUNCTION_DEFINITION)
{
	m_functionType = _functionType;
	m_infiniteParams = true;
	m_innerFunction = nullptr;
	m_innerFunctionDefinition = nullptr;
}

ScmObject_FunctionDefinition::ScmObject_FunctionDefinition(std::vector<std::shared_ptr<ScmObject_Symbol>> _params, std::shared_ptr<ScmObject_FunctionCall> _innerFunction) : ScmObject(ScmObjectType::FUNCTION_DEFINITION)
{
	m_functionType = ScmObject_FunctionDefinition::FunctionType::CUSTOM;

	m_infiniteParams = false;
	m_params = _params;
	m_innerFunction = _innerFunction;
}

ScmObject_FunctionDefinition::ScmObject_FunctionDefinition(std::vector<std::shared_ptr<ScmObject_Symbol>> _params, std::shared_ptr<ScmObject_FunctionDefinition> _innerFunctionDefinition) : ScmObject(ScmObjectType::FUNCTION_DEFINITION)
{
	m_functionType = ScmObject_FunctionDefinition::FunctionType::CUSTOM;

	m_infiniteParams = false;
	m_params = _params;
	m_innerFunctionDefinition = _innerFunctionDefinition;
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

std::shared_ptr<ScmObject> ScmObject_FunctionDefinition::getExecutable(const std::vector<std::shared_ptr<ScmObject>>& _args, std::shared_ptr<Environment> _parentEnv, std::shared_ptr<Environment> _closureEnv) const
{
	if (_args.size() != m_params.size() && !m_infiniteParams)
	{
		return std::make_shared<ScmObject_InternalError>("Argument count did not match parameter count. Expected " + std::to_string(m_params.size()) + " got " + std::to_string(_args.size()) + ".");
	}

	if (m_innerFunction != nullptr)
	{
		std::shared_ptr<Environment> env = std::make_shared<Environment>(_parentEnv);

		auto param = m_params.begin();
		auto arg = _args.begin();
		for (; arg != _args.end() && param != m_params.end(); ++arg, ++param)
		{
			env->addSymbol(*param, *arg);
		}

		return m_innerFunction->createFunctionExecution(env, std::make_shared<Environment>(m_functionDefinitionEnvironment));
	}
	else if (m_innerFunctionDefinition != nullptr)
	{
		std::shared_ptr<ScmObject_FunctionDefinition> newDef = std::make_shared<ScmObject_FunctionDefinition>(*m_innerFunctionDefinition);
		newDef->m_functionDefinitionEnvironment = Environment(m_functionDefinitionEnvironment);
		newDef->m_functionDefinitionEnvironment.setParent(_closureEnv);

		auto param = m_params.begin();
		auto arg = _args.begin();
		for (; arg != _args.end() && param != m_params.end(); ++arg, ++param)
		{
			newDef->m_functionDefinitionEnvironment.addSymbol(*param, *arg);
		}

		return newDef;
	}
	else
	{
		throw new std::exception("Function definition did not have an inner function or function definition. This should never happen and means there is an error in code.");
	}
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
	case FunctionType::SYNTAX_QUOTE:
		return "<Syntax:quote>";
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

			if (m_innerFunction != nullptr)
			{
				output += m_innerFunction->getOutputString();
			}
			else
			{
				output += m_innerFunctionDefinition->getOutputString();
			}

			output += ")";

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

	if (m_innerFunction != nullptr)
	{
		result = m_innerFunction->makeCons();
	}
	else
	{
		result = std::make_shared<ScmObject_Cons>(m_innerFunctionDefinition->getFunctionBody(), nullptr);
		result = std::make_shared<ScmObject_Cons>(m_innerFunctionDefinition->getFunctionArgList(), result);
		result = std::make_shared<ScmObject_Cons>(std::make_shared<ScmObject_Symbol>("lambda"), result);
	}

	// Wrap the cons in another cons to make it look like in class.
	result = std::make_shared<ScmObject_Cons>(result, nullptr);

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

const std::shared_ptr<ScmObject_Cons> ScmObject_FunctionDefinition::makeCons() const
{
	auto result = std::make_shared<ScmObject_Cons>(getFunctionArgList(), nullptr);
	result = std::make_shared<ScmObject_Cons>(m_innerFunctionDefinition->getFunctionArgList(), result);
	result = std::make_shared<ScmObject_Cons>(std::make_shared<ScmObject_Symbol>("lambda"), result);

	return result;
}