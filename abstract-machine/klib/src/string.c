#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t length = 0;
  int i = 0;
  while(s[i] != '\0'){
    i++;
    length++;
  }
  return length;
}

char *strcpy(char *dst, const char *src) {
  char *ret = dst;
  while((*dst++ = *src++) != '\0');
  return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
  if (n != 0) {
	  char *d = dst;
		const char *s = src;
		do {
			if ((*d++ = *s++) == 0) {
				/* NUL pad the remaining n-1 bytes */
				while (--n != 0)
					*d++ = 0;
				break;
			}
		} while (--n != 0);
	}
	return dst;
}

char *strcat(char *dst, const char *src) {
  char *tmp = dst;
  while (*dst) dst++;
  while ((*dst++ = *src++) != '\0');
  return tmp;
}

int strcmp(const char *s1, const char *s2) {
  unsigned char c1, c2;
  while (1) {
    c1 = *s1++;
    c2 = *s2++;
    if (c1 != c2) return c1 < c2 ? -1 : 1;
    if (!c1)  break;
  }
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  if (n == 0) return 0;
	do {
	  if (*s1 != *s2++) return (*(unsigned char *)s1 - *(unsigned char *)--s2);
		if (*s1++ == 0) break;
  }while (--n != 0);
	return 0;
}

void *memset(void *s, int c, size_t n) {
  void* ret = s;
  while(n--){
    *(char*)s = (char)c;
    s = (char*)s + 1;
  }
  return ret;
}

void *memmove(void *dst, const void *src, size_t n) {
  char *d = dst;   
  const char *s = src;
  if (d < s)
    while (n--)
      *d++ = *s++;
  else{
    const char *lasts = s + (n - 1);
    char *lastd = d + (n - 1);
    while (n--)
      *lastd-- = *lasts--;
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *d = out;
  const char *s = in;
  while (n--) *d++ = *s++;
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *str1 = s1;
	const unsigned char *str2 = s2;
  while (n-- > 0){
	  if (*str1++ != *str2++) return str1[-1] < str2[-1] ? -1 : 1;
  }
	return 0;
}

#endif
