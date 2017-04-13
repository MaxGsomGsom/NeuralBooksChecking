#ifndef EXCEPTIONS
#define EXCEPTIONS

#define WRONG_ARGS_EX(expr) if (!(expr)) throw runtime_error(("Wrong input args given to function " + string(__FUNCTION__) + " in " + string(__FILE__)).c_str());

#define OUT_OF_RANGE_EX(expr) if (!(expr)) throw runtime_error(("Vector out of range in function " + string(__FUNCTION__) + " in " + string(__FILE__)).c_str());

#define VECTOR_SIZE_EX(expr) if (!(expr)) throw runtime_error(("Vector size do not match with expected in function " + string(__FUNCTION__) + " in " + string(__FILE__)).c_str());

#include <stdexcept>
#include <string>

#endif // EXCEPTIONS

