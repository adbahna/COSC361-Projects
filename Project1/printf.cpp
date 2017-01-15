#include <printf.hpp>

int main() {
    int char_count = printf("Hello, world!\n");
    if (char_count == 14)
        printf("Correct number of bytes written!\n\n");

    printf("Int: %d\nFloat: %f\nHex: %x\n",1234,1.056432,0x1234f);

    return 0;
}

// TODO: implement the argument writing functions
int write_integer(int64_t arg) {

    return 0;
}
int write_float(double arg) {

    return 0;
}
int write_hex(uint64_t arg) {

    return 0;
}
int write_string(char* arg) {

    return 0;
}

/**
  Printf equivalent. Prints format and associated
  variables to stdout using write from <unistd.h>.
  @fmt - The string/format to print to the screen
  @returns - The number of bytes printed to the screen.

  Supports %d, %f, and %x (decimal, float, and hex).
  All variables for %d, %f, and %x must be 64-bits!
*/
int printf(const char *fmt, ...) {
    int char_count, start_index;
    int i;

    va_list args;
    va_start(args, fmt);

    char_count = 0;
    start_index = 0;
    // parse through the fmt string to find the arguments
    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {    // we found an argument, now we need to parse it
            write(1, (fmt+start_index), i-start_index);
            char_count += i-start_index;

            // TODO: currently assumes we have either %d, %f, %s, or %x. Add formatting!!
            i++;
            switch (fmt[i]) {
                case 'd':
                    char_count += write_integer(va_arg(args, int64_t));
                    break;
                case 'f':
                    char_count += write_float(va_arg(args, double));
                    break;
                case 'x':
                    char_count += write_hex(va_arg(args, uint64_t));
                    break;
                case 's':
                    char_count += write_string(va_arg(args, char*));
                    break;
            }
            i++;
            start_index = i;
        }
    }

    // write any leftover characters
    write(1, fmt+start_index, i-start_index);
    char_count += i-start_index;

    return char_count;
}

/**
  Same as printf, except prints to the string dest.
  @dest - The destination string to print to, buffer must be big enough
  to store "size" or size of the "fmt".
  @fmt - The string/format to store into "dest"
  @returns - The number of bytes stored in "dest"

  Supports %d, %f, and %x (decimal, float, and hex).
 All variables for %d, %f, and %x must be 64-bits!
*/
int snprintf(char *dest, size_t size, const char *fmt, ...) {

    return 0;
}


