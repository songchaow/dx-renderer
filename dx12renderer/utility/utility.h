#pragma once
#include <stdexcept>

#define ThrowIfFailed(f) if( (f) < 0) { throw std::runtime_error("API call failed");}