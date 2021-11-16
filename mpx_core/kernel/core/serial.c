/** @file */

/*
  ----- serial.c -----

  Description..: Contains methods and variables used for
    serial input and output.
    
*/

#include <stdint.h>
#include <string.h>

#include <core/io.h>
#include <core/serial.h>

#define NO_ERROR 0

// Active devices used for serial I/O
int serial_port_out = 0;
int serial_port_in = 0;

/*
  Procedure..: init_serial
  Description..: Initializes devices for user interaction, logging, ...
*/
int init_serial(int device)
{
  outb(device + 1, 0x00); //disable interrupts
  outb(device + 3, 0x80); //set line control register
  outb(device + 0, 115200/9600); //set bsd least sig bit
  outb(device + 1, 0x00); //brd most significant bit
  outb(device + 3, 0x03); //lock divisor; 8bits, no parity, one stop
  outb(device + 2, 0xC7); //enable fifo, clear, 14byte threshold
  outb(device + 4, 0x0B); //enable interrupts, rts/dsr set
  (void)inb(device);      //read bit to reset port
  return NO_ERROR;
}

/*
  Procedure..: serial_println
  Description..: Writes a message to the active serial output device.
    Appends a newline character.
*/
int serial_println(const char *msg)
{
  int i;
  for(i=0; *(i+msg)!='\0'; i++){
    outb(serial_port_out,*(i+msg));
  }
  outb(serial_port_out,'\r');
  outb(serial_port_out,'\n');  
  return NO_ERROR;
}

/*
  Procedure..: serial_print
  Description..: Writes a message to the active serial output device.
*/
int serial_print(const char *msg)
{
  int i;
  for(i=0; *(i+msg)!='\0'; i++){
    outb(serial_port_out,*(i+msg));
  }
  if (*msg == '\r') outb(serial_port_out,'\n');
  return NO_ERROR;
}

/*int (*student_read)(char *buffer, int *count);
  Procedure..: set_serial_out
  Description..: Sets serial_port_out to the given device address.
    All serial output, such as that from serial_println, will be
    directed to this device.
*/
int set_serial_out(int device)
{
  serial_port_out = device;
  return NO_ERROR;
}

/*
  Procedure..: set_serial_in
  Description..: Sets serial_port_in to the given device address.
    All serial input, such as console input via a virtual machine,
    QEMU/Bochs/etc, will be directed to this device.
*/
int set_serial_in(int device)
{
  serial_port_in = device;
  return NO_ERROR;
}

/**The polling function designs a way to capture the keyboard input */
int polling(char *buffer, int *count){
// insert your code to gather keyboard input via the technique of polling.
// You must validat each key and handle special keys such as delete, back space, and
// arrow keys
	klogv("*----------*");
	int index = 0;
	int size = 0;
	while(1){
	if(*count == 100){
	serial_println("");
	klogv("query too big for buffer");
	return 0;
	
	}
		if (inb(COM1+5)&1){
			char letter = inb(COM1);
			if ((((letter > 96)&&(letter < 123))||((letter > 47)&&(letter < 58))||((letter > 64)&&(letter < 91))||(letter == 32)||(letter == 47)||(letter == 61)||(letter == 59))&&(*count<100)){ // checks for letters, spaces, and numbers as well if the buffer size is below 100
				int tempindex = 0;
				if (index < size) {
					tempindex = size;
					buffer = buffer + ((size - index)); // sets the buffer to the end (size)
					while (tempindex != index) {
						
						char val = *buffer;  // char value = value in the buffer at the current index
						tempindex++; // increment the temp index by 1
						buffer = buffer + 1; // increment the buffer index by 1
						*buffer = val; // sets the value at the incremented buffer index to the vale that was held by the index before it
						buffer = buffer - 1;
						tempindex = tempindex - 1;
						if(tempindex != 0) { // checks to make sure the temp index is not at 0
						buffer = buffer - 1;
						tempindex = tempindex - 1;
						}
					}

					if (tempindex == index) { // if the temp index is now the same as the index
						char val = *buffer; // char value = value in the buffer at the current index
						tempindex++; // increment the temp index by 1
						buffer = buffer + 1; // increment the buffer index by 1
						*buffer = val; // sets the value at the incremented buffer index to the value that was held by the index before it
						buffer = buffer - 1; // sets the buffer back to the cursor index location
						tempindex = tempindex - 1; // sets the tempindex back to the cursor index location
						*buffer = letter; // changes the char at the index location in buffer
						


					}
					serial_print("\x1B[s"); // saves cursor position
					serial_print("\x1B[K"); // deletes the rest of the line starting from the index
					serial_print(buffer); // prints out the rest of the buffer starting from the current index
					serial_print("\x1B[u"); // restores cursor position
					size++; // increments the local size variable
					*count=size;
					if (index < 99) { // checks to make sure not to increment index if the size is 100
							index++;
							buffer = buffer + 1;
							serial_print("\x1B[C");	
						}
				}

				else{
				*buffer=letter; // sets the value of the current buffer address to the value of letter
				size = size+1; // increments the size of the buffer
				*count=size; // sets the buffer's count pointer (size) to the local variable size
				serial_print(buffer);
				buffer = buffer+1; //increment buffer address with size of char
				index = index+1;
				}
			}

			if (letter == 127) { // detects backspace
				if  (index != 0){
					char temp[strlen(buffer)]; // creates a temp string
					strcpy(temp,buffer); // sets the temp string to the buffer at the index onwards
					strcpy(buffer-1,temp); // sets buffer at the temp string minus 1 index position
					serial_print("\x1B[D");	// moves the cursor to the left 1 position
					serial_print("\x1B[s"); // saves the position of the cursor
					serial_print("\x1B[K"); // deletes the rest of the line starting from the index
					serial_print(temp); // prints the rest of the buffer
					serial_print("\x1B[u"); // restores cursor position
					index--;
					buffer = buffer -1;
					size--;
					*count=size;
				}

			}
			
			
			if(letter == 27){ // detects special characters
				letter=inb(COM1);
				if (letter=='['){
					letter=inb(COM1);
					
					if(letter=='C' && (index < *count)){ // right arrow
						serial_print("\x1B[C");	// moves cursor to the right 1 position
						index = index + 1; // moves the index to the right 1 position
						buffer = buffer + 1; // moves the buffer to the right 1 position
						}
					if(letter=='D' && (index > 0)){ // left arrow
						serial_print("\x1B[D");	// moves cursor to the left 1 position	
						index = index - 1; // moves the index to the left 1 position
						buffer = buffer - 1; // moves the buffer to the left 1 position
						}
					}

					if(letter == '3'){ // delete
						letter=inb(COM1);
						if(letter == '~'){
							if  (index != size){
							char temp[strlen(buffer)]; // creates a temp string
							strcpy(temp,buffer+1); // sets the temp string to the buffer at the index onwards
							strcpy(buffer,temp);  // sets buffer at the temp string
							serial_print("\x1B[s"); // saves cursor position
							serial_print("\x1B[K"); // deletes the rest of the line starting from the index
							serial_print(buffer); // prints out the rest of the buffer starting from the current index
							serial_print("\x1B[u"); // restores cursor position
							size--;
							*count=size;
								}
							}
						}
				}
			if (letter == 13) { // enter for returning statment
			serial_println("");
			return 0;
			}
		}
		
	}
		
		
	
	
	
// remove the following line after implementing your module, this is present
// just to allow the program to compile before R1 is complete
//strlen(buffer);

return 0;
}

