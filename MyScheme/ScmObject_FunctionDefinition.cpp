#include "ScmObject_FunctionDefinition.h"
#include "ScmObject_InternalError.h"
#include <string>

std::unordered_map<ScmObject_FunctionDefinition::FunctionType, ScmObject_FunctionDefinition*> ScmObject_FunctionDefinition::m_builtInFunctions;

ScmObject_FunctionDefinition::ScmObject_FunctionDefinition(ScmObject_FunctionDefinition::FunctionType _functionType) : ScmObject(ScmObjectType::FUNCTION_DEFINITION)
{
	m_functionType = _functionType;
	m_infiniteParams = true;
	m_innerFunction = nullptr;
}

ScmObject_FunctionDefinition::ScmObject_FunctionDefinition(std::vector<ScmObject_Symbol*> _params, ScmObject_FunctionCall* _innerFunction) : ScmObject(ScmObjectType::FUNCTION_DEFINITION)
{
	m_functionType = ScmObject_FunctionDefinition::FunctionType::CUSTOM;

	m_infiniteParams = false;
	m_params = _params;
	m_innerFunction = _innerFunction;
}

ScmObject_FunctionDefinition::ScmObject_FunctionDefinition(std::vector<ScmObject_Symbol*> _params, ScmObject_FunctionDefinition* _innerFunctionDefinition) : ScmObject(ScmObjectType::FUNCTION_DEFINITION)
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

ScmObject* ScmObject_FunctionDefinition::getExecutable(const std::vector<ScmObject*>& _args, Environment* _parentEnv, Environment* _closureEnv) const
{
	if (_args.size() != m_params.size() && !m_infiniteParams)
	{
		return new ScmObject_InternalError("Argument count did not match parameter count. Expected " + std::to_string(m_params.size()) + " got " + std::to_string(_args.size()) + ".");
	}

	if (m_innerFunction != nullptr)
	{
		Environment* env = new Environment(_parentEnv);

		auto param = m_params.begin();
		auto arg = _args.begin();
		for (; arg != _args.end() && param != m_params.end(); ++arg, ++param)
		{
			env->addSymbol(*param, *arg);
		}

		return m_innerFunction->createFunctionExecution(env, const_cast<Environment*>(&m_functionDefinitionEnvironment));
	}
	else if (m_innerFunctionDefinition != nullptr)
	{
		ScmObject_FunctionDefinition* newDef = new ScmObject_FunctionDefinition(*m_innerFunctionDefinition);
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

ScmObject* ScmObject_FunctionDefinition::copy() const
{
	return new ScmObject_FunctionDefinition(*this);
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
	case FunctionType::SYNTAX_DEFINE:
		return "<Syntax:define>";
	case FunctionType::SYNTAX_IF:
		return "<Syntax:if>";
	default:
		return "<Closure>";
	}
}