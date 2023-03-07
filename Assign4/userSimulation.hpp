#ifndef __USER_SIMULATION_HPP
#define __USER_SIMULATION_HPP

#include "sns.hpp"
#include <unistd.h>
#include <set>

void* userSimulator(void *);
extern ofstream& operator << (ofstream&, const Action&);

#endif