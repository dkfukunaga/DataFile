

#include "testItem.h"



TestItem::TestItem():
    name("TESTNAME"),
    int_num(42),
    long_num(-1),
    double_num(420.69),
    float_num(69.69) {
    buildArr(1);
};

TestItem::TestItem(std::string new_name):
    name(new_name),
    int_num(42),
    long_num(-1),
    double_num(420.69),
    float_num(69.69) {
    buildArr(1);
};

TestItem::TestItem(std::string new_name, int new_int, long long new_long, double new_double, float new_float):
    name(new_name),
    int_num(new_int),
    long_num(new_long),
    double_num(new_double),
    float_num(new_float) {
    buildArr(int_num);
}

void TestItem::serialize(DataFile &file){
    file.write(name);
    file.write(&int_num);
    file.write(&long_num);
    file.write(&double_num);
    file.write(&float_num);
    file.writeArray(int_arr, arr_len);
}

void TestItem::serialize(DataFile &file, long long pos) {
    file.setWritePos(pos);
    serialize(file);
}

void TestItem::deserialize(DataFile &file) {
    file.read(name);
    file.read(&int_num);
    file.read(&long_num);
    file.read(&double_num);
    file.read(&float_num);
    file.readArray(int_arr, arr_len);
}

void TestItem::deserialize(DataFile &file, long long pos) {
    file.setReadPos(pos);
    deserialize(file);
}

// Prints out a hex dump to the console from the provided DataFile
// 
// Reads entire file at once; may not be suitable for large file sizes.
void TestItem::hexDump(DataFile &file) {
    long long size = file.getFileSize();    // get file size to initialize buffer

    // initialize buffer and read the entire file in bytes
    std::vector<unsigned char> buff(size);
    file.readArray(buff.data(), size);

    long long address = 0;                      // counter for first byte of current line
    long long lines = ((size - 1) / 16) + 1;    // number of lines of 16 bytes + 1 for any extra

    // print hex dump header
    printf(" Address:    0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F\n");
    printf("================================================================================\n");

    // print the hex dump
    for (int line = 0; line < lines; ++line) {
        // print memory address in hex in an 8 wide column with leading 0's
        printf("|%08X| ", address);
        
        // loop through the next 8 bytes (if they exist) and print the bytes in
        // hex with a leading 0, or leave a blank space if that bye doens't exist
        for (int i = 0; i < 8; ++i) {
            if (address + i < size) {
                printf(" %02X", buff[address + i]);
            } else {
                printf("   ");
            }
        }

        // add a space between first 8 bytes
        printf(" ");

        // loop through the next 8 bytes (if they exist) and print the bytes in
        // hex with a leading 0, or leave a blank space if that bye doens't exist
        for (int i = 8; i < 16; ++i) {
            if (address + i < size) {
                printf(" %02X", buff[address + i]);
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
                if (buff[address + i] >= 32 && buff[address + i] <= 126) {
                    printf("%c", buff[address + i]);
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

    // print hex dump footer
    printf("================================================================================\n");
    printf("  Size: %d bytes\n\n", size);
}

int TestItem::getSize() {
    return name.length() + 2 + sizeof(int_num) + sizeof(long_num) +
           sizeof(double_num) + sizeof(float_num) + sizeof(int_arr);
}

void TestItem::buildArr(int num) {
    int_arr[0] = num;
    int_arr[1] = num;
    for (int i = 2; i < arr_len; ++i) {
        int_arr[i] = int_arr[i-1] + int_arr[i-2];
    }
}


std::string TestItem::toString() {
    std::string array_string = "[" + std::to_string(int_arr[0]);

    for (int i = 1; i < arr_len; ++i) {
        array_string += ", " + std::to_string(int_arr[i]);
    }
    array_string += "]";

    return std::string("Name:      \"" + name + "\"\n" +
                       "Integer:   " + std::to_string(int_num) + "\n" +
                       "Long Long: " + std::to_string(long_num) + "\n" +
                       "Double:    " + std::to_string(double_num) + "\n" +
                       "Float:     " + std::to_string(float_num) + "\n" +
                       "Array:     " + array_string + "\n");
}