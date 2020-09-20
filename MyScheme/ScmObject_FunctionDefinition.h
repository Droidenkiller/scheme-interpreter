#pragma once
#include "ScmObject.h"
#include "ScmObject_FunctionCall.h"
#include "ScmObject_Cons.h"
#include "Environment.h"
#include <vector>
#include <exception>
#include <memory>

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
		BUILT_IN_CONS,
		BUILT_IN_CAR,
		BUILT_IN_CDR,
		BUILT_IN_EQUALS,
		BUILT_IN_GT,
		BUILT_IN_LT,
		BUILT_IN_STRINGQ,
		BUILT_IN_CONSQ,
		BUILT_IN_NUMBERQ,
		BUILT_IN_FUNCTIONQ,
		BUILT_IN_USER_DEFINED_FUNCTIONQ,
		BUILT_IN_PRINT,
		BUILT_IN_DISPLAY,
		BUILT_IN_FUNCTION_BODY,
		BUILT_IN_FUNCTION_ARG_LIST,
		BUILT_IN_READ_LINE,
		BUILT_IN_LOAD,
		SYNTAX_QUOTE,
		SYNTAX_LAMBDA,
		SYNTAX_DEFINE,
		SYNTAX_IF,
		SYNTAX_SET,
		SYNTAX_BEGIN
	};

private:
	FunctionType m_functionType;
	bool m_infiniteParams = false;
	std::vector<std::shared_ptr<ScmObject_Symbol>> m_params;
	std::shared_ptr<Environment> m_functionDefinitionEnvironment;
	std::vector<std::shared_ptr<ScmObject>> m_innerFunctions;

	static std::unordered_map<FunctionType, std::shared_ptr<ScmObject_FunctionDefinition>> m_builtInFunctions;

public:
	// Only supposed to be used inside of the class. Public due to shared_ptr.
	ScmObject_FunctionDefinition(FunctionType _functionType);

	// Constructs function definition with parameters.
	ScmObject_FunctionDefinition(std::vector<std::shared_ptr<ScmObject_Symbol>> _params, std::vector<std::shared_ptr<ScmObject>>& _innerFunctions, std::shared_ptr<Environment> _parentEnv, std::shared_ptr<Environment> _closureEnv);

	//// Constructs function definition with an infinite amount of parameters.
	//ScmObject_FunctionDefinition(ScmObject_FunctionCall* _innerFunction);

	FunctionType getFunctionType() const;

	// Executes the function defined by this definition.
	std::vector<std::shared_ptr<ScmObject>> getExecutable(const std::vector<std::shared_ptr<ScmObject>>& _args) const;

	bool equals(const ScmObject* _other) const;

	std::shared_ptr<const ScmObject> copy() const;

	const std::string getOutputString() const;

	const std::string getDisplayString() const;

	const std::shared_ptr<ScmObject_Cons> getFunctionBody() const;

	const std::shared_ptr<ScmObject_Cons> getFunctionArgList() const;

	// This is deprecated and not needed anymore.
	//const std::shared_ptr<ScmObject_Cons> makeCons() const;

	template<FunctionType T>
	static std::shared_ptr<ScmObject_FunctionDefinition> getFunctionDefinition()
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
			m_builtInFunctions[T] = std::make_shared<ScmObject_FunctionDefinition>(T);
			return m_builtInFunctions[T];
		}
	}
};

