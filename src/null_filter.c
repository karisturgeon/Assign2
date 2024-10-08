
#include "../include/null_filter.h"
#include <stdlib.h>

void null_filter(const char *c)
{
    if(c != NULL)
    {
        return;
    }
}

void null_filter_wrapper(char *c)
{
    null_filter((const char *)c);
}
