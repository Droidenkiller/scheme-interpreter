#pragma once
#include "ScmObject.h"
#include "ScmObject_FunctionCall.h"
#include "Environment.h"
#include <memory>

std::shared_ptr<ScmObject> exec(std::shared_ptr<ScmObject_FunctionExecution> _funcExec);