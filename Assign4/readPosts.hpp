#ifndef __READ_POSTS_HPP
#define __READ_POSTS_HPP

#include "sns.hpp"

extern ofstream& operator << (ofstream&, const Action&);
extern ostream& operator << (ostream&, const Action&);

#endif