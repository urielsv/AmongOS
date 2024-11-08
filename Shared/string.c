// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <string.h>
#include <stdint.h>


int8_t strcmp(const char* s1, const char* s2) {
    while(*s1 == *s2 && *s1 != '\0'){
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

uint64_t strlen(char* buff)
{
    uint64_t ans = 0;
    while(buff[ans] != 0)
        ans++;
    return ans;
}

void strcpy(char* dest, char* src)
{
    while(*src != 0)
    {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = 0;
}

char *strchr(char* str, char c)
{
    while(*str != 0)
    {
        if(*str == c)
            return str;
        str++;
    }
    return 0;
}
