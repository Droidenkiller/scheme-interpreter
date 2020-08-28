#ifdef OLD_BUILT_INS

#pragma once
#include "ScmObjectIncludes.h"
#include <vector>

bool tryExecBuiltIn(const ScmObject_FunctionCall* _functionCall, Environment* _env, Environment* _closureEnv, ScmObject*& _out);

ScmObject* builtInAdd(const ScmObject_FunctionCall* _functionCall, Environment* _env, Environment* _closureEnv);

ScmObject* builtInSubtract(const ScmObject_FunctionCall* _functionCall);

ScmObject* builtInMultiply(const ScmObject_FunctionCall* _functionCall);

ScmObject* builtInDivide(const ScmObject_FunctionCall* _functionCall);

// Defines variable in environment.
ScmObject* syntaxDefine(const ScmObject_FunctionCall* _functionCall, Environment* _env, Environment* _closureEnv);

ScmObject* syntaxIf(const ScmObject_FunctionCall* _functionCall, Environment* _env, Environment* _closureEnv);

#endif