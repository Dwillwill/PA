#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
    char buffer[128];
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

void itoa(unsigned int n, char * buf){
    int i;
    if(n < 10){
        buf[0] = n + '0';
        buf[1] = '\0';
        return;
    }
    itoa(n / 10, buf);
    for(i = 0; buf[i] != '\0'; i++);
    buf[i] = (n % 10) + '0';
    buf[i+1] = '\0';
}

int vsprintf(char *str, const char *fmt, va_list ap) {
    int n;
    unsigned long int u_long_int;
    char * s;
    char buf[64];
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
                    itoa(n, buf);
                    // printf("case d buf=[%s]\n", buf);
                    memcpy(str, buf, strlen(buf));
                    str += strlen(buf);
                    break;
                }
                case 'l': {
                    u_long_int = va_arg(ap, unsigned long int);
                    if (u_long_int < 0) {
                        *str = '-';
                        str++;
                        u_long_int = -u_long_int;
                    }
                    // printf("case d n=[%d]\n", n);
                    itoa(u_long_int, buf);
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
