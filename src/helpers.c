int lwmqtt_read_int(unsigned char **pptr) {
  // get pointer
  unsigned char *ptr = *pptr;

  // read two byte integer
  int num = 256 * (*ptr) + (*(ptr + 1));

  // adjust pointer
  *pptr += 2;

  return num;
}

char lwmqtt_read_char(unsigned char **pptr) {
  // read single char
  char chr = **pptr;

  // adjust pointer
  (*pptr)++;

  return chr;
}

void lwmqtt_write_char(unsigned char **pptr, unsigned char chr) {
  // write single char
  **pptr = chr;

  // adjust pointer
  (*pptr)++;
}

void lwmqtt_write_int(unsigned char **pptr, int num) {
  // write first byte
  **pptr = (unsigned char)(num / 256);

  // adjust pointer
  (*pptr)++;

  // write second byte
  **pptr = (unsigned char)(num % 256);

  // adjust pointer
  (*pptr)++;
}