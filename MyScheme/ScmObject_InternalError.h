#pragma once
#include "ScmObject.h"
#include <string>
#include <memory>

class ScmObject_InternalError :
    public ScmObject
{
private:
    std::string m_message;

public:
    ScmObject_InternalError(std::string _message);

    std::string getMessage() const;

    bool equals(const ScmObject* _other) const;

    std::shared_ptr<const ScmObject> copy() const;

    std::string const getOutputString() const;

	const std::string getDisplayString() const;
};

