/*
 * str.c
 * Twilio Breakout SDK
 *
 * Copyright (c) 2018 Twilio, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file str.c - string and operations - not null terminated
 */

#include "str.h"

#include <stdlib.h>
#include <ctype.h>

void str_skipover_prefix(str *x, str prefix) {
  if (!x) return;
  if (str_equal_prefix(*x, (prefix))) {
    x->s += prefix.len;
    x->len -= prefix.len;
  }
}

int str_tok(str src, const char *sep, str *dst) {
  unsigned int i, j, is_sep, sep_len;
  unsigned int start;
  if (!src.len || !sep || !dst) return 0;
  sep_len = strlen(sep);
  if (!sep_len) return 0;
  if (dst->s) {
    if (dst->s < src.s || dst->s > src.s + src.len || dst->s + dst->len > src.s + src.len) {
      //      LOG(L_ERR, "The token parameter must either be an empty string on first call, or the last token!");
      return 0;
    }
  }
  if (!dst->s)
    start = 0;
  else
    start = dst->s + dst->len - src.s;
  for (i = start; i < src.len; i++) {
    is_sep = 0;
    for (j = 0; j < sep_len; j++)
      if (src.s[i] == sep[j]) {
        is_sep = 1;
        break;
      }
    if (!is_sep) {
      dst->s = src.s + i;
      for (i = i + 1; i < src.len; i++) {
        is_sep = 0;
        for (j = 0; j < sep_len; j++)
          if (src.s[i] == sep[j]) {
            is_sep = 1;
            break;
          }
        if (is_sep) break;
      }
      dst->len = src.s + i - dst->s;
      return 1;
    }
  }
  return 0;
}

int str_tok_with_empty_tokens(str src, const char *sep, str *dst) {
  unsigned int i, j, is_sep, sep_len;
  unsigned int start;
  if (!src.len || !sep || !dst) return 0;
  sep_len = strlen(sep);
  if (!sep_len) return 0;
  if (dst->s) {
    if (dst->s < src.s || dst->s > src.s + src.len || dst->s + dst->len > src.s + src.len) {
      //      LOG(L_ERR, "The token parameter must either be an empty string on first call, or the last token!");
      return 0;
    }
  }
  if (!dst->s)
    start = 0;
  else
    start = dst->s + dst->len - src.s;
  for (i = start; i < src.len; i++) {
    is_sep = 0;
    for (j = 0; j < sep_len; j++)
      if (src.s[i] == sep[j]) {
        is_sep = 1;
        break;
      }
    if (!is_sep) {
      dst->s = src.s + i;
      for (i = i + 1; i < src.len; i++) {
        is_sep = 0;
        for (j = 0; j < sep_len; j++)
          if (src.s[i] == sep[j]) {
            is_sep = 1;
            break;
          }
        if (is_sep) break;
      }
      dst->len = src.s + i - dst->s;
      return 1;
    } else {
      // peek at the next token and if separator, return empty token
      if (i + 1 >= src.len) return 0;
      is_sep = 0;
      for (j = 0; j < sep_len; j++)
        if (src.s[i + 1] == sep[j]) {
          is_sep = 1;
          break;
        }
      if (is_sep) {
        dst->s   = src.s + i + 1;
        dst->len = 0;
        return 1;
      }
    }
  }
  return 0;
}

long int str_to_long_int(str x, int base) {
  char buf[65];
  if (x.len > 64) {
    //    LOG(L_ERR, "The give string is too long to convert - %d > 64\r\n", x.len);
    return 0;
  }
  if (x.len > 1 && x.s[0] == '"' && x.s[x.len - 1] == '"') {
    memcpy(buf, x.s + 1, x.len - 2);
    buf[x.len - 2] = 0;
  } else {
    memcpy(buf, x.s, x.len);
    buf[x.len] = 0;
  }
  return strtol(buf, 0, base);
}

uint32_t str_to_uint32_t(str x, int base) {
  char buf[33];
  if (x.len > 32) {
    //    LOG(L_ERR, "The give string is too long to convert - %d > 32\r\n", x.len);
    return 0;
  }
  if (x.len > 1 && x.s[0] == '"' && x.s[x.len - 1] == '"') {
    memcpy(buf, x.s + 1, x.len - 2);
    buf[x.len - 2] = 0;
  } else {
    memcpy(buf, x.s, x.len);
    buf[x.len] = 0;
  }
  return strtoul(buf, 0, base);
}

double str_to_double(str x) {
  char buf[65];
  if (x.len > 64) {
    //    LOG(L_ERR, "The give string is too long to convert - %d > 64\n", x.len);
    return 0;
  }
  memcpy(buf, x.s, x.len);
  buf[x.len] = 0;
  return strtod(buf, 0);
}

void uint8_t_to_binary_str(uint8_t x, str_mut *dst, uint8_t precision) {
  int ndx = 0;
  if (precision == 0 || precision > 8) precision = 8;
  for (ndx = 0; ndx <= (precision - 1); ndx++) {
    dst->s[(precision - 1) - ndx] = ((x >> ndx) & 1) ? '1' : '0';
  }
  dst->len = precision;
}

int hex_to_int(char c) {
  switch (c) {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'A':
    case 'a':
      return 10;
    case 'B':
    case 'b':
      return 11;
    case 'C':
    case 'c':
      return 12;
    case 'D':
    case 'd':
      return 13;
    case 'E':
    case 'e':
      return 14;
    case 'F':
    case 'f':
      return 15;
    default:
      return -1;
  }
  return -1;
}

unsigned int hex_to_str(char *dst, unsigned int max_dst_len, str src) {
  unsigned int len = 0;
  int hn, ln;
  unsigned int i;
  if (src.len % 2 != 0) return 0;
  for (i = 0; i < src.len && i < max_dst_len * 2; i += 2) {
    hn = hex_to_int(src.s[i]) * 16;
    if (hn < 0) {
      return 0;
    }
    ln = hex_to_int(src.s[i + 1]);
    if (ln < 0) {
      return 0;
    }
    *((uint8_t *)(dst + len)) = hn + ln;
    len++;
  }
  return len;
}

static char hex_char[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

unsigned int str_to_hex(char *dst, unsigned int max_dst_len, str src) {
  unsigned int len = 0;
  unsigned int i;
  for (i = 0; i < src.len && len < max_dst_len; i++) {
    dst[len++] = hex_char[(*((uint8_t *)src.s + i) >> 4) & 0x0F];
    dst[len++] = hex_char[(*((uint8_t *)src.s + i)) & 0x0F];
  }
  return len;
}

int str_find(str x, str y) {
  unsigned int i;
  if (!y.len) return -1;
  for (i = 0; i <= x.len - y.len; i++)
    if (memcmp(x.s + i, y.s, y.len) == 0) return i;
  return -1;
}

int str_find_char(str x, const char *y) {
  unsigned int i;
  int len = strlen(y);
  if (!len) return -1;
  for (i = 0; i <= x.len - len; i++)
    if (memcmp(x.s + i, y, len) == 0) return i;
  return -1;
}

void str_strip(str *s) {
  while (s->len > 0 && isspace(s->s[s->len - 1]))
    s->len--;
}
