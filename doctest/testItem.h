

#ifndef TEST_ITEM_H
#define TEST_ITEM_H

#include "..\src\DataFile.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

struct TestItem {
    const static int foot_len = 6;

    unsigned int    test_id = 0xFECAA70C;
    std::string     test_str;
    long long       test_long;
    float           test_float;
    short           test_foot[foot_len] = {1, 16, 256, 4096, -1, 15001};


    TestItem();
    TestItem(std::string new_str, long long new_long, float new_float);

    void            serialize(DataFile &file, long long pos);
    void            serialize(DataFile &file);
    void            deserialize(DataFile &file);
    void            deserialize(DataFile &file, long long pos);

    int             getSize();
    std::string     toString();

};


#endif