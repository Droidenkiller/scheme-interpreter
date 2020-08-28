#pragma once
#include "ScmObject.h"
#include <string>
class ScmObject_InternalError :
    public ScmObject
{
private:
    std::string m_message;

public:
    ScmObject_InternalError(std::string _message);

    std::string getMessage() const;

    bool equals(const ScmObject* _other) const;

    ScmObject* copy() const;

    std::string const getOutputString() const;
};

