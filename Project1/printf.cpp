#include "printf.hpp"

// TODO: Delete this main before turning in project
int main() {

    // tests for printf to see char_count is correct
    int char_count = printf("%s\n","Hello, world!123456789");
    if (char_count == 23) printf("Correct number of bytes written!\n");
    char_count = printf("%d\n",123456789L);
    if (char_count == 10) printf("Correct number of bytes written!\n");
    char_count = printf("%d\n",-123456789L);
    if (char_count == 11) printf("Correct number of bytes written!\n");
    char_count = printf("%.4f\n",123456789.123456789);
    if (char_count == 15) printf("Correct number of bytes written!\n");
    char_count = printf("%.4f\n",-123456789.123456789);
    if (char_count == 16) printf("Correct number of bytes written!\n");
    char_count = printf("%x\n",0x123456789);
    if (char_count == 10) printf("Correct number of bytes written!\n");
    char_count = printf("%d\n",9223372036854775807L);
    if (char_count == 20) printf("Correct number of bytes written!\n");
    char_count = printf("%d\n",-9223372036854775807L);
    if (char_count == 21) printf("Correct number of bytes written!\n");

    char dest[100];
    char_count = snprintf(dest,50,"Hello, my name is who my name is%.4f", 12345.1234567);
    printf("%s %d\n",dest,char_count);
//    if (char_count == 22) printf("Correct number of bytes written!\n");

    return 0;
}

//Function that writes integer argument to stdout
const char n[] = "0123456789";
int write_integer(int64_t argument) {

    int char_count = 0;
    int is_neg = 0;

    //set posiitive, output arrays, and incrementer
    if (argument < 0)
    {
        is_neg = 1;
        argument = -argument;
    }

    char out[22];
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
    write(1, out, char_count);

    return char_count;
}

//Function that writes floating point char array to printf with precision parameter
const char f[] = "0123456789";
int write_float(double arg, int precision) {

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

    char out[48];
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
    write(1, out, char_count);

    return char_count;
}

//Function to write hex char array to printf function
const char a[] = "0123456789ABCDEF";
int write_hex(uint64_t arg) {

    int char_count = 0;

    //incrementer and output char array, to write
    int i = 16;
    char out[17];

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

    //Writing char array
    write(1,out,char_count);
    return char_count;
}

//Function that writes char array with char array parameter
int write_string(char* arg) {
    int char_count = 0;

    //simply counts, no need to copy to another array
    for (char_count = 0; arg[char_count] != '\0'; char_count++) {}

    //writes the arg
    write(1,arg,char_count);
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
    int i;

	

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

            // TODO: currently assumes we have either %d, %f, %s, or %x. Add formatting!!
            i++;
            switch (fmt[i]) {
                case 'd':
                    char_count += write_integer(va_arg(args, int64_t));
                    break;
                case '.':
                    precision = (fmt[++i] - '0');
                    char_count += write_float(va_arg(args, double), precision);
                    i++;
                    break;
                case 'f':
                    char_count += write_float(va_arg(args, double), 6);
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
    int char_count, actual_char_count, bytes_to_write, start_index, precision;
    int i, j, c;
    int n = size-1, is_neg;

	const char a1[] = "0123456789";	

    va_list args;
    va_start(args, fmt);

    precision = 6;
    char_count = 0;
    actual_char_count = 0;
    start_index = 0;
    // parse through the fmt string to find the arguments
    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {    // we found an argument, now we need to parse it
            bytes_to_write = (i-start_index) < (n-actual_char_count) ? i-start_index : n-actual_char_count;
            for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(fmt+start_index+j);
            actual_char_count += bytes_to_write;
            char_count += i-start_index;

            i++;
            switch (fmt[i]) {
                case 'd':
					long long dArgument;

					dArgument = va_arg(args, int64_t);

			    	c = 0;
    				is_neg = 0;

			   		//set posiitive, output arrays, and incrementer
					if (dArgument < 0)
					{
						is_neg = 1;
						dArgument *= -1;
					}

					char dout[22];
					j = 21;

					//Sets value to out based on based 10 char array above
					do {
						dout[j] = a1[dArgument % 10];
						j--;
						dArgument = dArgument / 10;
					}while (dArgument > 0);


				//Set values of out based on loop above

					if (is_neg)
						c++;

					while(++j < 22)
						dout[c++] = dout[j];

				//Set negative char in front
					if (is_neg)
						dout[0] = '-';

				//Set ending sentinel char and write to output
					
    				bytes_to_write = c < (n-actual_char_count) ? c : n-actual_char_count;
				    for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(dout+j);
				    actual_char_count += bytes_to_write;
					
					char_count += c;
        	        break;
                case '.':
				{
                    precision = (fmt[++i] - '0');

 				    c = 0;
				    is_neg = 0;
				    long long argument;
					double arg;
					
					arg = va_arg(args, double);
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

				    char pout[48];
					j = 47;
				    //decimal counter
				    int dcount;

				    //extracting characters from integer part of arg
				    do {
				        pout[j] = a1[argument % 10];
				        j--;	
				        argument = argument / 10;
				    }while (argument > 0);

				    //putting chars of integer argument to output array
    				if (is_neg)
				        c++;

				    while(++j < 48)
				        pout[c++] = pout[j];

				    //adding floating point char
				    pout[c++] = '.';

				    //collecting decimal digits until precision is reached
				    for (dcount = 0; dcount < precision + 1; dcount++) {
				        fraction *= 10;
				        pout[c] = ((long long)fraction) + '0';
				        c++;
				        fraction -= (long long) fraction;
				    }

   					 //Checking for rounding
				    if (pout[c - 1] >= '5')
				        pout[c - 2] += 1;

				    //adding negative char to output if negative
				    if (is_neg)
				        pout[0] = '-';

				    pout[c--] = '\0';
    				
					bytes_to_write = c < (n-actual_char_count) ? c : n-actual_char_count;
				    for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(pout+j);
				    actual_char_count += bytes_to_write;
					char_count += c;

                    // char_count += write_float(va_arg(args, double), precision);
                    i++;
                    break;
				}
                case 'f':
           		{
			         precision = 6;

 				    c = 0;
				    is_neg = 0;
					
					double arg;
					long long argument;
					double fraction;

					arg = va_arg(args, double);
				    is_neg = 0;
				    argument = (long long) arg;

				    if (argument < 0)
    				{
				        argument *= -1;
				        arg *= -1;
				        is_neg = 1;
    				}

				    //extracting the decimal from arg
				    fraction = (arg - (long long)argument);


				    char fout[48];
					j = 47;
				    //decimal counter
				    int dcount;

				    //extracting characters from integer part of arg
				    do {
				        fout[j] = a1[argument % 10];
				        j--;	
				        argument = argument / 10;
				    }while (argument > 0);

				    //putting chars of integer argument to output array

    				if (is_neg)
				        c++;

				    while(++j < 48)
				        fout[c++] = fout[j];

				    //adding floating point char
				    fout[c++] = '.';

				    //collecting decimal digits until precision is reached
				    for (dcount = 0; dcount < precision + 1; dcount++) {
				        fraction *= 10;
				        fout[c] = ((long long)(fraction)) + '0';
				        c++;

				        fraction -= (long long) fraction;
				    }
					
   					 //Checking for rounding
				    if (fout[c - 1] >= '5')
				        fout[c - 2] += 1;

				    //adding negative char to output if negative
				    if (is_neg)
				        fout[0] = '-';
						
					fout[c--] = '\0';

    				bytes_to_write = c < (n-actual_char_count) ? c : n-actual_char_count;
				    for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(fout+j);
				    actual_char_count += bytes_to_write;
					char_count += c;
                }   
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
    bytes_to_write = (i-start_index) < (n-actual_char_count) ? i-start_index : n-actual_char_count;

    for (j = 0; j < bytes_to_write; j++) *(dest+actual_char_count+j) = *(fmt+start_index+j);
    actual_char_count += bytes_to_write;
    char_count += i-start_index;

	printf("%d %d %d\n", bytes_to_write, actual_char_count, char_count);
	printf("%s\n", dest);
    dest[actual_char_count] = '\0';

    return char_count;
}
