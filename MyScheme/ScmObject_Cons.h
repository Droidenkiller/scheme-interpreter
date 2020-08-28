#pragma once
#include "ScmObject.h"
#include <string>

class ScmObject_Cons :
    public ScmObject
{
private:
    ScmObject* car;
    ScmObject* cdr;

public:
    ScmObject_Cons();

    const ScmObject* getCar() const;
    const ScmObject* getCdr() const;

    void setCar(ScmObject* _car);
    void setCdr(ScmObject* _cdr);

    bool equals(const ScmObject* _other) const;

    ScmObject* copy() const;

    std::string const getOutputString() const;
};

