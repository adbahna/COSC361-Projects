/////////////////////////////////////////////////////////////////////////////////////////////
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
//=                                                                                       +//
//-				  CS-361 PROJECT 1 // IMPLEMENTING BASIC PRINTF AND SNPRINTF 	          -//
//=				  -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=              =//
//-																						  -//
//=						 =  Contributors: Michael Goin, Alex Bahna  =				   	  =//
//-																						  -//
//=						  	=  Completed: January 25 2017  = 						      =//
//-																						  -//
//=									 Description:									 	  =//
//-																						  -//
//=																						  =//
//-	   = Implementation of printf and snprintf (albeit not robust) in language C++		  -//
//=    = Utilizes printf.hpp, by using its "write" function								  =//
//-	       - @Parameters: write(output (stdout = 1),  char array, length to write         -//
//=	   = All arguments assumed to be 64-bit, and are treated as such in each function	  =//
//-		   - @Arguments: "%d, %f, %.1-6f, %s, %x" 										  -//
//=																						  =//
//+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
/////////////////////////////////////////////////////////////////////////////////////////////
#include "printf.hpp"



//Function that writes integer argument to stdout
const char n[] = "0123456789";
int write_integer(int64_t argument, char* out) {

    int char_count = 0;
    int is_neg = 0;

    //set posiitive, output arrays, and incrementer
    if (argument < 0)
    {
        is_neg = 1;
        argument = -argument;
    }

    int i = 21;

    //Sets value to out based on based 10 char array above
    do {
        out[i] = n[argument % 10];
        i--;
        argument = argument / 10;
    }while (argument > 0);


    //Set values of out based on loop above

    if (is_neg)
        char_count++;

    while(++i < 22)
        out[char_count++] = out[i];

    //Set negative char in front
    if (is_neg)
        out[0] = '-';

    //Set ending sentinel char and write to output
    out[char_count] = '\0';

    return char_count;
}

//Function that writes floating point char array to printf with precision parameter
const char f[] = "0123456789";
int write_float(double arg, int precision, char* out) {

    //Set incrementer, and set argument to first part of
    //decimal value
    int char_count;
    int is_neg;
    long long argument;

    char_count = 0;
    is_neg = 0;
    argument = (long long) arg;

    if (argument < 0)
    {
        argument *= -1;
        arg *= -1;
        is_neg = 1;
    }

    //extracting the decimal from arg
    double fraction = (arg - (long long)argument);

    int i = 47;

    //decimal counter
    int dcount;

    //extracting characters from integer part of arg
    do {
        out[i] = f[argument % 10];
        i--;
        argument = argument / 10;
    }while (argument > 0);

    //putting chars of integer argument to output array

    if (is_neg)
        char_count++;

    while(++i < 48)
        out[char_count++] = out[i];

    //adding floating point char
    out[char_count++] = '.';

    //collecting decimal digits until precision is reached
    for (dcount = 0; dcount < precision + 1; dcount++) {
        fraction *= 10;
        out[char_count] = ((long long)fraction) + '0';
        char_count++;

        fraction -= (long long)fraction;
    }

    //Checking for rounding
    if (out[char_count - 1] >= '5')
        out[char_count - 2] += 1;

    //adding negative char to output if negative
    if (is_neg)
        out[0] = '-';

    out[char_count--] = '\0';

    return char_count;
}

//Function to write hex char array to printf function
const char a[] = "0123456789ABCDEF";
int write_hex(uint64_t arg, char* out) {

    int char_count = 0;

    //incrementer and output char array, to write
    int i = 16;

    //extract base 16 chars from input arg, based on array above write_hex
    do {
        out[i] = a[arg % 16];
        i--;
        arg = arg/16;
    }while(arg > 0);

    //Putting array in correct order while incrementing char count
    while( ++i < 17){
        out[char_count++] = out[i];
    }

    //Sentinelizing array at the end of arg
    out[char_count] = '\0';

    return char_count;
}

//Function that writes char array with char array parameter
int write_string(char* arg) {
    int char_count = 0;

    //simply counts, no need to copy to another array
    for (char_count = 0; arg[char_count] != '\0'; char_count++) {}

    return char_count;
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
    int char_count, start_index, precision;
    int i, c;


    va_list args;
    va_start(args, fmt);

    precision = 6;
    char_count = 0;
    start_index = 0;
    // parse through the fmt string to find the arguments
    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {    // we found an argument, now we need to parse it
            write(1, (fmt+start_index), i-start_index);
            char_count += i-start_index;

            i++;
 			//switch case that passes the written argument based on type
            switch (fmt[i]) {
                case 'd':
                    char tmp_d[100];
                    c = write_integer(va_arg(args, int64_t),tmp_d);
                    write(1,tmp_d,c);
                    char_count += c;
                    break;
                case '.':
                    char tmp_fp[100];

					//'.' specifies precision up 6 digits, so precision needds to be set
					//		Default precision is 6, when just an f appears
                    precision = (fmt[++i] - '0');

                    c = write_float(va_arg(args, double), precision,tmp_fp);
                    write(1,tmp_fp,c);
                    char_count += c;
                    i++;
                    break;
                case 'f':
                    char tmp_f[100];
                    c = write_float(va_arg(args, double), 6,tmp_f);
                    write(1,tmp_f,c);
                    char_count += c;
                    break;
                case 'x':
                    char tmp_x[100];
                    c = write_hex(va_arg(args, uint64_t),tmp_x);
                    write(1,tmp_x,c);
                    char_count += c;
                    break;
                case 's':
                    char *tmp_s = va_arg(args, char*);
                    c = write_string(tmp_s);
                    write(1,tmp_s,c);
                    char_count += c;
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


/*
   Same as printf, except prints to the string dest.
   @dest - The destination string to print to, buffer must be big enough
   to store "size" or size of the "fmt".
   @fmt - The string/format to store into "dest"
   @returns - The number of bytes stored in "dest"

   Supports %d, %f, and %x (decimal, float, and hex).
   All variables for %d, %f, and %x must be 64-bits!
   */
int snprintf(char *dest, size_t size, const char *fmt, ...) {
    int char_count, actual_char_count, bytes_to_write, start_index, precision;
    int i, j, c;
    int n = size-1;


    va_list args;
    va_start(args, fmt);

    precision = 6;
    char_count = 0;
    actual_char_count = 0;
    start_index = 0;
    c = 0;
    // parse through the fmt string to find the arguments
    for (i = 0; fmt[i] != '\0';) {
        if (fmt[i] == '%') {    // we found an argument, now we need to parse it
            bytes_to_write = (i-start_index) < (n-actual_char_count) ? i-start_index : n-actual_char_count;
            for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(fmt+start_index+j);
            actual_char_count += bytes_to_write;
            char_count += i-start_index;

            i++;
			//Same switch as printf, but with different counting / writing
            switch (fmt[i]) {
                case 'd':
                    char tmp_d[100];
                    c = write_integer(va_arg(args, int64_t),tmp_d);
                    //writes the arg
                    bytes_to_write = c < (n-actual_char_count) ? c : n-actual_char_count;
                    for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(tmp_d+j);
                    actual_char_count += bytes_to_write;
                    char_count += c;
                    break;
                case '.':
                    char tmp_fp[100];

                    precision = (fmt[++i] - '0');
                    c = write_float(va_arg(args, double), precision,tmp_fp);
                    //writes the arg
                    bytes_to_write = c < (n-actual_char_count) ? c : n-actual_char_count;
                    for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(tmp_fp+j);
                    actual_char_count += bytes_to_write;
                    char_count += c;
                    i++;
                    break;
                case 'f':
                    char tmp_f[100];
                    c = write_float(va_arg(args, double),6,tmp_f);
                    //writes the arg
                    bytes_to_write = c < (n-actual_char_count) ? c : n-actual_char_count;
                    for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(tmp_f+j);
                    actual_char_count += bytes_to_write;
                    char_count += c;
                    break;
                case 'x':
                    char tmp_x[100];
                    c = write_hex(va_arg(args, uint64_t), tmp_x);
                    //writes the arg
                    bytes_to_write = c < (n-actual_char_count) ? c : n-actual_char_count;
                    for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(tmp_x+j);
                    actual_char_count += bytes_to_write;
                    char_count += c;
                    break;
                case 's':
                    char *tmp_s = va_arg(args, char*);
                    c = write_string(tmp_s);
                    //writes the arg
                    bytes_to_write = c < (n-actual_char_count) ? c : n-actual_char_count;
                    for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(tmp_s+j);
                    actual_char_count += bytes_to_write;
                    char_count += c;
                    break;
            }
            i++;
            start_index = i;
        }
    }

    // write any leftover characters
    bytes_to_write = (i-start_index) < (n-actual_char_count) ? i-start_index : n-actual_char_count;
    for (j = 0; j < bytes_to_write; j++) {*(dest+actual_char_count+j) = *(fmt+start_index+j);}
    actual_char_count += bytes_to_write;
    char_count += i-start_index;

    dest[actual_char_count] = '\0';

    return char_count;
}
