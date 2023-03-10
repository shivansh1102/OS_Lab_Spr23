#ifndef __PUSH_UPDATES_HPP
#define __PUSH_UPDATES_HPP

#include "sns.hpp"
#include <cassert>
extern ofstream& operator << (ofstream&, const Action&);
extern ostream& operator << (ostream&, const Action&);

#endif