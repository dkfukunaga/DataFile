

#include "testItem.h"



TestItem::TestItem():
    name("TESTNAME"),
    int_num(42),
    double_num(420.69) {
    buildArr(1);
};

TestItem::TestItem(std::string new_name, int new_int, double new_double):
    name(new_name),
    int_num(new_int),
    double_num(new_double) {
    buildArr(int_num);
}

void TestItem::serialize(DataFile &file){
    file.write(name);
    file.write(&int_num);
    file.write(&double_num);
    file.write(int_arr);
}

void TestItem::deserialize(DataFile &file) {
    file.read(&name);
    file.read(&int_num);
    file.read(&double_num);
    file.read<int, 10>(int_arr);
}

// Prints out a hex dump to the console from the provided DataFile
// with the following format:
// 00000000  00 a2 00 01 03 00 00 00  00 73 77 72 63 6f 6d 6d  |.........swrcomm|
// 00000010  61 6e 64 6f 00 73 77 72  63 6f 6d 6d 61 6e 64 6f  |ando.swrcommando|
// address in hex, 8 bytes in hex, 8 bytes in hex, preceding 16 bytes in ascii
//
// Reads entire file at once; may not be suitable for large file sizes.
void TestItem::hexDump(DataFile &file) {
    long long size = file.getFileSize();    // get file size to initialize buffer

    // initialize buffer and read the entire file as unsigned bytes
    std::vector<uint8_t> buffer(size);
    file.read(buffer.data());

    long long address = 0;                  // counter for first byte of current line
    long long lines = (size / 16) + 1;      // number of lines of 16 bytes + 1 for any extra

    // print the hex dump
    for (int line = 0; line < lines; ++line) {
        // print memory address in hex in an 8 wide column with leading 0's
        printf("%08X ", address);
        
        // loop through the next 8 bytes (if they exist) and print the bytes in
        // hex with a leading 0, or leave a blank space if that bye doens't exist
        for (int i = 0; i < 8; ++i) {
            if (address + i < size) {
                printf(" %02X", buffer[address + i]);
            } else {
                printf("   ");
            }
        }

        // add a space between first 8 bytes
        printf(" ");

        // loop through the next 8 bytes (if they exist) and print the bytes in
        // hex with a leading 0, or leave a blank space if that bye doens't exist
        for (int i = 0; i < 8; ++i) {
            if (address + i < size) {
                printf(" %02X", buffer[address + i]);
            } else {
                printf("   ");
            }
        }
        
        // print 2 spaces and a | to separate ascii representation
        printf("  |");

        // loop through the same 16 bytes (if they exist) as above, printing the
        // ascii character if it is printable, '.' if it is unprintable, or an empty
        // space if the byte doesn't exist
        for (int i = 0; i < 16; ++i) {
            if (address + i < size){
                if (buffer[address + i] >= 32 && buffer[address + i] <= 126) {
                    printf("%c", buffer[address + i]);
                } else {
                    printf(".");
                }
            } else {
                printf(" ");
            }
        }

        // print | to close off ascii section and go to the next line
        printf("|\n");

        // increment address by one line
        address += 16;
    }
}

void TestItem::buildArr(int num) {
    int_arr[0] = num;
    int_arr[1] = num;
    for (int i = 2; i < 10; ++i) {
        int_arr[i] = int_arr[i-1] + int_arr[i-2];
    }
}


std::string TestItem::toString() {
    std::string array_string = "[" + std::to_string(int_arr[0]);

    for (int i = 1; i < 10; ++i) {
        array_string += ", " + std::to_string(int_arr[i]);
    }
    array_string += "]";

    return std::string("Name:    " + name + "\n" +
                       "Integer: " + std::to_string(int_num) + "\n" +
                       "Double:  " + std::to_string(double_num) + "\n" +
                       "Array:   " + array_string + "\n");
}