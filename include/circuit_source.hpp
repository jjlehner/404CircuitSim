#ifndef GUARD_CIRCUIT_SOURCE_HPP
#define GUARD_CIRCUIT_SOURCE_HPP

#include <string>

#include "circuit_structure.hpp"

class Circuit::CurrentSource : public Circuit::Component {
public:
    CurrentSource( std::string name, float value ) : Component( name, value ) {};
};

#endif