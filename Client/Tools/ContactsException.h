

#include <string>

class ContactsException
{
private:
    std::string __message;

public:
    ContactsException(std::string message = "A problem")
        : __message(message) {}
    std::string what() const
    {
        return __message;
    }
};