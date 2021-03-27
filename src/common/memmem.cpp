#include <stdint.h>
#include <memory.h>

int MemMem(const char* mem1, const char* mem2, int len1, int len2)
{
    if (len1 > 0 && len2 > 0)
    {
        int len = len1 - len2 + 1;
        if (len > 0)
        {
            for (int i = 0; i < len; ++i)
            {
                if (!memcmp(&mem1[i], mem2, (size_t)len2))
                {
                    return i;
                }
            }
        }
    }

    return -1;
}

