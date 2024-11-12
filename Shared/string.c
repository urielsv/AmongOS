// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
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

char to_lower(char c) {
    if (c >= 'a' && c <= 'z')
        return c + 32;
    return c;
}