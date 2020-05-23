#ifndef API_HH
#define API_HH

#include <stdexcept>

namespace quantum {

class Exception : public std::exception
{
    public:
        Exception( const std::string &message );
        const char* what() const noexcept override;
    private:
        std::string message_;
};

}

#endif