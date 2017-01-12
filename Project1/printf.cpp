/**
  Printf equivalent. Prints format and associated
  variables to stdout using write from <unistd.h>.
  @fmt - The string/format to print to the screen
  @returns - The number of bytes printed to the screen.

  Supports %d, %f, and %x (decimal, float, and hex).
  All variables for %d, %f, and %x must be 64-bits!
*/
int printf(const char *fmt, ...);
/**
 Same as printf, except prints to the string dest.
 @dest - The destination string to print to, buffer must be big enough
         to store "size" or size of the "fmt".
 @fmt - The string/format to store into "dest"
 @returns - The number of bytes stored in "dest"

 Supports %d, %f, and %x (decimal, float, and hex).
 All variables for %d, %f, and %x must be 64-bits!
*/
int snprintf(char *dest, size_t size, const char *fmt, ...);


