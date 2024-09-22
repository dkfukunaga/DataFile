

#ifndef TEST_ITEM_H
#define TEST_ITEM_H

#include "..\src\DataFile.h"
#include <string>
#include <vector>

struct TestItem {
    std::string     name;
    int             int_num;
    double          double_num;
    int             arr[10];

    TestItem();
    TestItem(std::string new_name, int new_int, double new_double);

    void            serialize(DataFile &file);
    void            deserialize(DataFile &file);
    void            hexDump(DataFile &file);

    void            buildArr(int num);
    std::string     toString();
};


#endif