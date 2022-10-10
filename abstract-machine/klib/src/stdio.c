#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
    char buffer[4096];
    va_list ap;
    int ret = -1;
    va_start(ap, fmt);
    ret = vsprintf(buffer, fmt, ap);
    va_end(ap);
    char *out = buffer;
    while(*out != '\0'){
        putch(*out);
        out++;
    }
    return ret;
    // panic("Not implemented");
}

void itoa(unsigned long n, char * buf, int base){
    int i;
    if(n < base){
        buf[0] = "0123456789abcdef"[n % base];
        buf[1] = '\0';
        return;
    }
    itoa(n / base, buf, base);
    for(i = 0; buf[i] != '\0'; i++);
    buf[i] = "0123456789abcdef"[n % base];
    buf[i+1] = '\0';
}

int vsprintf(char *str, const char *fmt, va_list ap) {
    int n;
    unsigned long long_int;
    char * s;
    char buf[4096];
    memset(buf, 0, sizeof (buf));
    while(*fmt != '\0') {
        if(*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd': {
                    n = va_arg(ap, int);
                    if (n < 0) {
                        *str = '-';
                        str++;
                        n = -n;
                    }
                    // printf("case d n=[%d]\n", n);
                    itoa(n, buf, 10);
                    // printf("case d buf=[%s]\n", buf);
                    memcpy(str, buf, strlen(buf));
                    str += strlen(buf);
                    break;
                }
                case 'l': {
                    long_int = va_arg(ap, unsigned long);
                    // printf("case d n=[%d]\n", n);
                    itoa(long_int, buf, 10);
                    // printf("case d buf=[%s]\n", buf);
                    memcpy(str, buf, strlen(buf));
                    str += strlen(buf);
                    break;
                }
                case 's': {
                    s = va_arg(ap, char *);
                    memcpy(str, s, strlen(s));
                    str += strlen(s);
                    break;
                }
                case 'x': {
                    long_int = va_arg(ap, unsigned long);
                    *(str++) = '0';
                    *(str++) = 'x';
                    // printf("case d n=[%d]\n", n);
                    itoa(long_int, buf, 16);
                    // printf("case d buf=[%s]\n", buf);
                    memcpy(str, buf, strlen(buf));
                    str += strlen(buf);
                    break;

                }
            }
        }else {
            *str = *fmt;
            str++;
        }
        fmt++;
    }
    *str = '\0';
    return 0;
}

int sprintf(char *out, const char *fmt, ...) {
    va_list ap;
    int ret = -1;
    va_start(ap, fmt);
    ret = vsprintf(out, fmt, ap);
    va_end(ap);
    return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
