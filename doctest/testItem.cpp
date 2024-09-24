

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