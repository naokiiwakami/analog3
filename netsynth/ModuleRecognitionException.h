#ifndef _modulerecognitionexception_h_
#define _modulerecognitionexception_h_

#include <string>

class ModuleRecognitionException
{
public:
    ModuleRecognitionException(const std::string& message) {
        m_message = message;
    }

    const std::string& what() {
        return m_message;
    }

    std::string m_message;
};

#endif
