#ifndef EXCEPTIONS
#define EXCEPTIONS

#include <stdexcept>

using namespace std;

namespace Neuronets
{

class WrongInputArgs: public logic_error
{
public:
    WrongInputArgs(string funcName) : logic_error("Wrong input args given to function " + funcName) {}
};

class OutOfRange: public logic_error
{
public:
    OutOfRange(string funcName) : logic_error("Vector out of range in function " + funcName) {}
};

class WrongInputVectorSize: public logic_error
{
public:
    WrongInputVectorSize(string funcName) : logic_error("Vector size don't match with expected in function " + funcName) {}
};

}
#endif // EXCEPTIONS

