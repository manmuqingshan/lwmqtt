#include <string.h>

#include "helpers.h"

lwmqtt_string_t lwmqtt_str(const char *str) { return (lwmqtt_string_t){(int)strlen(str), (char *)str}; }

int lwmqtt_strcmp(lwmqtt_string_t *a, const char *b) {
  // get length of b
  size_t len = strlen(b);

  // otherwise check if length matches
  if (len != a->len) {
    return -1;
  }

  // compare memory
  return strncmp(a->data, b, len);
}

bool lwmqtt_read_string(lwmqtt_string_t *str, void **buf, void *buf_end) {
  // check if at lest 2 bytes
  if (buf_end - (*buf) <= 1) {
    return false;
  }

  // read length
  int len = lwmqtt_read_num(buf);

  // check if string end is overflowing the end pointer
  if (&(*buf)[len] > buf_end) {
    return false;
  }

  // set string
  str->len = len;
  str->data = (char *)*buf;

  // advance pointer
  *buf += str->len;

  return true;
}

void lwmqtt_write_string(void **pptr, lwmqtt_string_t string) {
  // write length prefixed string if length is given
  if (string.len > 0) {
    lwmqtt_write_num(pptr, string.len);
    memcpy(*pptr, string.data, string.len);
    *pptr += string.len;
    return;
  }

  // write zero
  lwmqtt_write_num(pptr, 0);
}

int lwmqtt_read_num(void **buf) {
  // get array
  unsigned char *ary = *buf;

  // read two byte integer
  int num = 256 * ary[0] + ary[1];

  // adjust pointer
  *buf += 2;

  return num;
}

void lwmqtt_write_num(void **pptr, int num) {
  // get array
  unsigned char *ary = *pptr;

  // write bytes
  ary[0] = (unsigned char)(num / 256);
  ary[1] = (unsigned char)(num % 256);

  // adjust pointer
  *pptr += 2;
}

unsigned char lwmqtt_read_byte(void **buf) {
  // get array
  unsigned char *ary = *buf;

  // adjust pointer
  *buf += 1;

  return ary[0];
}

void lwmqtt_write_byte(void **buf, unsigned char chr) {
  // get array
  unsigned char *ary = *buf;

  // write single char
  *ary = chr;

  // adjust pointer
  *buf += 1;
}

int lwmqtt_read_varnum(void **buf, int buf_len) {
  // get array
  unsigned char *ary = *buf;

  // prepare last digit
  unsigned char digit;

  // prepare multiplier
  int multiplier = 1;

  // prepare length
  int len = 0;

  // initialize number
  int num = 0;

  // decode variadic number
  do {
    // increment length
    len++;

    // return error if buffer is to small
    if (buf_len < len) {
      return -1;
    }

    // return error if the length has overflowed
    if (len > 4) {
      return -2;
    }

    // read digit
    digit = ary[len - 1];

    // add digit to number
    num += (digit & 127) * multiplier;

    // increase multiplier
    multiplier *= 128;
  } while ((digit & 128) != 0);

  // adjust pointer
  *buf += len;

  return num;
}

int lwmqtt_write_varnum(void **buf, int buf_len, int num) {
  // get array
  unsigned char *ary = *buf;

  // init len counter
  int len = 0;

  // encode variadic number
  do {
    // return error if buffer is to small
    if (buf_len < len) {
      return -1;
    }

    // return error if the length has overflowed
    if (len > 4) {
      return -2;
    }

    // calculate current digit
    unsigned char digit = (unsigned char)(num % 128);

    // change remaining length
    num /= 128;

    // set the top bit of this digit if there are more to encode
    if (num > 0) {
      digit |= 0x80;
    }

    // write digit
    ary[len++] = digit;
  } while (num > 0);

  // adjust pointer
  *buf += len;

  return 0;
}
