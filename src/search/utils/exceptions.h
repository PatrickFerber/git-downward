#ifndef UTILS_EXCEPTIONS_H
#define UTILS_EXCEPTIONS_H

namespace utils {
class Exception {
public:
    virtual ~Exception() = default;
    virtual void print() const = 0;
};
}

#endif
