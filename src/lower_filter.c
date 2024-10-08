
#include "../include/lower_filter.h"
#include <ctype.h>

void lower_filter(char *c)
{
    *c = (char)tolower(*c);
}
