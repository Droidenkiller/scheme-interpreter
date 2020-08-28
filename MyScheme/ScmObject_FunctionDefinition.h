#pragma once
#include "ScmObject.h"
#include "ScmObject_FunctionCall.h"
#include "Environment.h"
#include <vector>
#include <exception>

class ScmObject_FunctionDefinition :
	public ScmObject
{
public:
	enum class FunctionType {
		CUSTOM,
		BUILT_IN_ADD,
		BUILT_IN_SUBTRACT,
		BUILT_IN_MULTIPLY,
		BUILT_IN_DIVIDE,
		BUILT_IN_EQUALS,
		BUILT_IN_GT,
		BUILT_IN_LT,
		SYNTAX_DEFINE,
		SYNTAX_IF
	};

private:
	FunctionType m_functionType;
	bool m_infiniteParams = false;
	std::vector<ScmObject_Symbol*> m_params;
	ScmObject_FunctionCall* m_innerFunction;
	Environment m_functionDefinitionEnvironment;
	// In case the inner function is a lambda. We have an inner function definition.
	ScmObject_FunctionDefinition* m_innerFunctionDefinition;
	static std::unordered_map<FunctionType, ScmObject_FunctionDefinition*> m_builtInFunctions;

	ScmObject_FunctionDefinition(FunctionType _functionType);

public:
	// Constructs function definition with parameters.
	ScmObject_FunctionDefinition(std::vector<ScmObject_Symbol*> _params, ScmObject_FunctionCall* _innerFunction);

	ScmObject_FunctionDefinition(std::vector<ScmObject_Symbol*> _params, ScmObject_FunctionDefinition* _innerFunctionDefinition);

	//// Constructs function definition with an infinite amount of parameters.
	//ScmObject_FunctionDefinition(ScmObject_FunctionCall* _innerFunction);

	FunctionType getFunctionType() const;

	// Executes the function defined by this definition.
	ScmObject* getExecutable(const std::vector<ScmObject*>& _args, Environment* _parentEnv, Environment* _closureEnv) const;

	bool equals(const ScmObject* _other) const;

	ScmObject* copy() const;

	const std::string getOutputString() const;

	template<FunctionType T>
	static ScmObject_FunctionDefinition* getFunctionDefinition()
	{
		if (T == FunctionType::CUSTOM)
		{
			throw new std::exception("Cannot get custom function definition in getFunctionDefinition. This is an error in code.");
		}

		auto it = m_builtInFunctions.find(T);

		if (it != m_builtInFunctions.end())
		{
			return it->second;
		}
		else
		{
			m_builtInFunctions[T] = new ScmObject_FunctionDefinition(T);
			return m_builtInFunctions[T];
		}
	}
};

