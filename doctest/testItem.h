

#ifndef TEST_ITEM_H
#define TEST_ITEM_H

#include "..\src\DataFile.h"
#include <string>
#include <vector>

struct TestItem {
    const static int arr_len = 17;
    std::string     name;
    int             int_num;
    long long       long_num;
    double          double_num;
    float           float_num;
    int             int_arr[arr_len] = {0};

    TestItem();
    TestItem(std::string new_name);
    TestItem(std::string new_name, int new_int, long long new_long, double new_double, float new_float);

    void            serialize(DataFile &file, long long pos);
    void            serialize(DataFile &file);
    void            deserialize(DataFile &file);
    void            deserialize(DataFile &file, long long pos);

    int             getSize();
    void            buildArr(int num);
    std::string     toString();

};


#endif