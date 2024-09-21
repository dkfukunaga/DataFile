

#ifndef TEST_ITEM_H
#define TEST_ITEM_H

#include "..\src\DataFile.h"
#include <string>

struct Item {
    std::string     name;
    int             int_num;
    double          double_num;
    int             arr[10];

    Item();
    Item(std::string new_name, int new_int, double new_double);

    void            serialize(DataFile &file);
    void            deserialize(DataFile &file);

    std::string     toString();
};


#endif