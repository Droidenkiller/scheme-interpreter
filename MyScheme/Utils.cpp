#include "Utils.h"

void stringReplace(string& _string, string _stringToReplace, string _newString)
{
    size_t pos = _string.find(_stringToReplace);
    // Repeat till end is reached
    while (pos != std::string::npos)
    {
        _string.replace(pos, _stringToReplace.size(), _newString);
        pos = _string.find(_stringToReplace, pos + _newString.size());
    }
}