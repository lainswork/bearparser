#include "Util.h"
#include <stdarg.h>

using namespace pe_util;

#define MAX_LINE 255

bool Logger::append(dbg_level lvl, const char* format, ...)
{
    if (lvl > DBG_LVL) {
        return false;
    }
    if (format == NULL) {
        return false;
    }
    va_list argptr;
    // Initializing arguments to store all values after format
    va_start(argptr, format);

    char line[MAX_LINE + 1];
    memset(line, 0, MAX_LINE + 1);

    int printed = vsnprintf(line, MAX_LINE, format, argptr);

    //cleaning up the list:
    va_end(argptr);
    if (printed <= 0) return false;

    const char *prefixes[D_LVL_COUNT] = { "ERROR", "WARNING", "INFO" };
    if (static_cast<unsigned>(lvl) > static_cast<unsigned>(D_LVL_COUNT)) {
        lvl = Logger::D_ERROR;
    }

    fprintf(stderr, "[%s] %s\n", prefixes[lvl], line);
    return true;
}

bool pe_util::isStrLonger(const char *inp, size_t maxLen)
{
    for (size_t i = 0; i < maxLen; i++ ) {
        if (inp[i] == '\0') return false;
    }
    return true;
}

size_t pe_util::getAsciiLen(const char *inp, size_t maxInp, bool acceptNotTerminated)
{
    size_t i = 0;
    for (; i < maxInp; i++) {
        const char c = inp[i];
        if (c == '\0') return i; //end of string
        if (!IS_PRINTABLE(c) && !IS_ENDLINE(c)) return 0;
    }
    if (acceptNotTerminated) return i;
    return 0;
}

size_t pe_util::getAsciiLenW(const WORD *inp, size_t maxInp, bool acceptNotTerminated)
{
    size_t i = 0;
    for (; i < maxInp; i++) {
        const WORD w = inp[i];
        if (w == 0) return i; //end of string
        if (!IS_PRINTABLE(w) && !IS_ENDLINE(w)) return 0;
    }
    if (acceptNotTerminated) return i;
    return 0;
}

bool pe_util::hasNonPrintable(const char *inp, size_t maxInp)
{
    unsigned int i = 0;
    for ( i = 0; i < maxInp; i++) {
        char c = inp[i];
        if (c == '\0') break; //end of string
        if (!IS_PRINTABLE(c)) return true;
    }
    return false;
}

size_t pe_util::forwarderNameLen(const char* fPtr, size_t bufSize)
{
    if (!fPtr || bufSize == 0) return 0;
    
    // names can be also mangled, i.e. MSVCRT.??0__non_rtti_object@std@@QAE@ABV01@@Z
    bool has_dot = false;
    size_t len = 0;
    while ((*fPtr >= 'a' && *fPtr <= 'z')
            || (*fPtr >= 'A' && *fPtr <= 'Z')
            || (*fPtr >= '0' && *fPtr <= '9')
            || (*fPtr == '.')
            || (*fPtr == '_')
            || (*fPtr == '#') 
            || (*fPtr == '@')
            || (*fPtr == '?')
            || (*fPtr == '-'))
    {
        if (*fPtr == '.') has_dot = true;
        len++;
        if ((--bufSize) == 0) break;
        fPtr++;
    }
    if (*fPtr == '\0') {
        if (!has_dot) {
            return 0; //this is not a valid forwarder
        }
        return len;
    }
    return 0;
}

size_t pe_util::noWhiteCount(char *buf, size_t bufSize) {
    size_t cntr = 0;
    size_t i = 0;
    for (i = 0; i < bufSize; i++) {
        if (IS_PRINTABLE(buf[i]) && buf[i] != ' ')
            cntr++;
    }
    return cntr;
}

size_t pe_util::noWhiteCount(std::string s)
{
    size_t bufSize = s.length();
    size_t cntr = 0;
    size_t i = 0;
    for (i = 0; i < bufSize; i++) {
        if (IS_PRINTABLE(s[i]) && s[i] != ' ')
            cntr++;
    }
    return cntr;
}

bool pe_util::isSpaceClear(void* ptr, uint64_t size)
{
    BYTE* testblock = (BYTE*) calloc(size, sizeof(BYTE));
    bool isClear = true;
    if (memcmp (testblock, ptr, size)) {
        isClear = false;
    }
    free(testblock);
    return isClear;
}

bool pe_util::isHexChar(char c)
{
    if (isdigit(c)) return true;
    if (c >= 'A' && c <= 'F') return true;
    if (c >= 'a' && c <= 'f') return true;
    return false;
}

void pe_util::hexdump(BYTE *buf, size_t bufSize, size_t pad)
{
    if (buf == NULL) return;
    printf("\n---\n");
    for (size_t i = 0; i < bufSize; i++) {
        if (i % pad == 0) printf("\n");
        printf("0x%02X ", buf[i]);
    }
    printf("\n---\n");
}

bool pe_util::endsWith(std::string str, std::string endStr)
{
    if (str.length() < endStr.length()) {
        return false;
    } 
    size_t pos = str.length() - endStr.length();
    std::string str3 = str.substr(pos);   
    if ( str3 == endStr ) {
        return true;
    }
    return false;
}
