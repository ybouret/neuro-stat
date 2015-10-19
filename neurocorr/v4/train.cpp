#include "train.hpp"

Train:: ~Train() throw()
{
}

Train:: Train(size_t n) : _Train(0,as_capacity)
{
}

Train:: Train(const Train &other) : _Train(other)
{
}

Train:: Train() throw() : _Train() {}
