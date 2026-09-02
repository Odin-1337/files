#ifndef crtH
#define crtH

#define to_lower(text) ((text >= (char*)'A' && text <= (char*)'Z') ? (text + 32) : text)

inline unsigned long strtoul(
    const char* str,
    char** endptr,
    int base
) {
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')
        str++;

    bool negative = false;
    if (*str == '-') {
        negative = true;
        str++;
    }
    else if (*str == '+') {
        str++;
    }

    if (base == 0) {
        if (*str == '0') {
            str++;
            if (*str == 'x' || *str == 'X') {
                base = 16;
                str++;
            }
            else {
                base = 8;
            }
        }
        else {
            base = 10;
        }
    }
    else if (base == 16) {
        if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
            str += 2;
        }
    }

    unsigned long result = 0;
    bool valid_digit_found = false;

    while (*str) {
        int digit;

        if (*str >= '0' && *str <= '9') {
            digit = *str - '0';
        }
        else if (*str >= 'a' && *str <= 'z') {
            digit = *str - 'a' + 10;
        }
        else if (*str >= 'A' && *str <= 'Z') {
            digit = *str - 'A' + 10;
        }
        else {
            break;
        }

        if (digit >= base) {
            break;
        }

        valid_digit_found = true;

        if (result > (ULONG_MAX - digit) / base) {
            result = ULONG_MAX;
            break;
        }

        result = result * base + digit;
        str++;
    }

    if (endptr) {
        *endptr = const_cast<char*>(valid_digit_found ? str : str - valid_digit_found);
    }

    return negative ? static_cast<unsigned long>(-static_cast<long>(result)) : result;
}

[[ nodiscard ]] 
inline char chrlwr(
    char c
) {
    if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
    return c;
}

[[ nodiscard ]] 
inline int stricmp(
    const char* cs, const char* ct
) {
    if (cs && ct) {
        while (chrlwr(*cs) == chrlwr(*ct)) {
            if (*cs == 0 && *ct == 0) return 0;
            if (*cs == 0 || *ct == 0) break;
            cs++;
            ct++;
        }
        return chrlwr(*cs) - chrlwr(*ct);
    }
    return -1;
}

[[ nodiscard ]]
inline const char* strstr(
    const char* haystack,
    const char* needle
) {
    if (!haystack || !needle)
        return nullptr;

    if (!*needle)
        return haystack;

    const char* p1 = haystack;
    while (*p1) {
        const char* p1_begin = p1;
        const char* p2 = needle;

        while (*p1 && *p2 && (*p1 == *p2)) {
            p1++;
            p2++;
        }

        if (!*p2)
            return p1_begin;

        p1 = p1_begin + 1;
    }

    return nullptr;
}

[[ nodiscard ]]
inline const wchar_t* wcsstr(
    const wchar_t* haystack,
    const wchar_t* needle
) {
    if (!haystack || !needle)
        return nullptr;

    if (*needle == L'\0')
        return haystack;

    for (const wchar_t* h = haystack; *h != L'\0'; ++h)
    {
        const wchar_t* a = h;
        const wchar_t* b = needle;

        while (*a != L'\0' && *b != L'\0' && *a == *b)
        {
            ++a;
            ++b;
        }

        if (*b == L'\0')
            return h;
    }

    return nullptr;
}

[[nodiscard]]
inline int tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}

[[nodiscard]]
inline char* strtolower(char* str) {
    for (char* p = str; *p != '\0'; ++p) {
        *p = static_cast<char>(tolower(*p));
    }
    return str;
}

[[ nodiscard ]] 
inline std::int32_t strncmp(
    const char* s1,
    const char* s2,
    size_t n
) {
    if (n == 0)
        return (0);
    do {
        if (*s1 != *s2++)
            return (*(unsigned char*)s1 - *(unsigned char*)--s2);
        if (*s1++ == 0)
            break;
    } while (--n != 0);
    return (0);
}

[[ nodiscard ]]
inline std::int32_t strcmp(
    const char* string,
    const char* string_cmp
) {
    while (*string != '\0')
    {
        if (*string != *string_cmp)
            break;
        string++;
        string_cmp++;
    }
    return *string - *string_cmp;
}

[[ nodiscard ]]
inline std::size_t strlen(
    const char* str
) {
    const char* s;
    for (s = str; *s; ++s);
    return (s - str);
}

[[ nodiscard ]] 
inline int memcmp(
    const void* s1,
    const void* s2,
    size_t n
) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* end1 = p1 + n;
    const unsigned char* p2 = (const unsigned char*)s2;
    int                   d = 0;
    for (;;) {
        if (d || p1 >= end1) break;
        d = (int)*p1++ - (int)*p2++;
        if (d || p1 >= end1) break;
        d = (int)*p1++ - (int)*p2++;
        if (d || p1 >= end1) break;
        d = (int)*p1++ - (int)*p2++;
        if (d || p1 >= end1) break;
        d = (int)*p1++ - (int)*p2++;
    }
    return d;
}

[[ nodiscard ]]
inline void* memcpy(
    void* dest,
    const void* src,
    size_t len
) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while (len--)
        *d++ = *s++;
    return dest;
}

[[ nodiscard ]] 
inline void* memset(
    void* dest,
    std::uint8_t c,
    size_t count
) {
    size_t blockIdx;
    size_t blocks = count >> 3;
    size_t bytesLeft = count - (blocks << 3);
    std::uint64_t cUll =
        c
        | (((std::uint64_t)c) << 8)
        | (((std::uint64_t)c) << 16)
        | (((std::uint64_t)c) << 24)
        | (((std::uint64_t)c) << 32)
        | (((std::uint64_t)c) << 40)
        | (((std::uint64_t)c) << 48)
        | (((std::uint64_t)c) << 56);

    std::uint64_t* destPtr8 = (std::uint64_t*)dest;
    for (blockIdx = 0; blockIdx < blocks; blockIdx++) destPtr8[blockIdx] = cUll;

    if (!bytesLeft) return dest;

    blocks = bytesLeft >> 2;
    bytesLeft = bytesLeft - (blocks << 2);

    std::uint32_t* destPtr4 = (std::uint32_t*)&destPtr8[blockIdx];
    for (blockIdx = 0; blockIdx < blocks; blockIdx++) destPtr4[blockIdx] = (std::uint32_t)cUll;

    if (!bytesLeft) return dest;

    blocks = bytesLeft >> 1;
    bytesLeft = bytesLeft - (blocks << 1);

    std::uint16_t* destPtr2 = (std::uint16_t*)&destPtr4[blockIdx];
    for (blockIdx = 0; blockIdx < blocks; blockIdx++) destPtr2[blockIdx] = (std::uint16_t)cUll;

    if (!bytesLeft) return dest;

    std::uint8_t* destPtr1 = (std::uint8_t*)&destPtr2[blockIdx];
    for (blockIdx = 0; blockIdx < bytesLeft; blockIdx++) destPtr1[blockIdx] = (std::uint8_t)cUll;

    return dest;
}

[[ nodiscard ]]
inline int wcscmp(
    const wchar_t* s1,
    const wchar_t* s2
) {
    while (*s1 == *s2++)
        if (*s1++ == '\0')
            return (0);

    return (*(const unsigned int*)s1 - *(const unsigned int*)--s2);
}

#endif crtH // crtH