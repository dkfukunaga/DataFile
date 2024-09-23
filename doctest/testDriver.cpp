

#include "testItem.cpp"
#include <iostream>

int main() {
    TestItem item_to_write_1("omg, it works! Amazing!!!!");
    TestItem item_to_write_2("Let's try with two items and see what happens.");
    item_to_write_2.int_num = 42069;
    item_to_write_2.double_num = 123.456789;
    item_to_write_2.float_num = 2.2222221;
    item_to_write_2.buildArr(3);
    TestItem item_to_write_3("THREE ITEMS NOW?");
    item_to_write_3.int_num = 333333;
    item_to_write_3.long_num = -333333;
    item_to_write_3.double_num = 0.0007;
    item_to_write_3.float_num = 0.01;
    item_to_write_3.buildArr(117);

    std::cout << "Item to write 1: \n" << item_to_write_1.toString() << "\n\n";
    std::cout << "Item to write 2: \n" << item_to_write_2.toString() << "\n\n";
    std::cout << "Item to write 3: \n" << item_to_write_3.toString() << "\n\n";
    std::cout << "Item 1 size: " << item_to_write_1.getSize() << "\n";
    std::cout << "Item 2 size: " << item_to_write_2.getSize() << "\n";
    std::cout << "Item 3 size: " << item_to_write_3.getSize() << "\n";
    std::cout << "Total size : " << item_to_write_1.getSize() + item_to_write_2.getSize()
              + item_to_write_3.getSize() << "\n\n";

    // DataFile file("test.dat");
    DataFile file;
    // file.setFileExtension(".dat2");
    file.setFileName("test");
    file.open();

    item_to_write_1.serialize(file);
    item_to_write_2.serialize(file);
    item_to_write_3.serialize(file);
    file.setReadPos(0);
    item_to_write_1.hexDump(file);

    std::cout << "\n";
    TestItem item_to_read_1;
    TestItem item_to_read_2;
    TestItem item_to_read_3;
    TestItem item_to_read_4;
    TestItem item_to_read_5;
    file.setReadPos(0);
    item_to_read_1.deserialize(file);
    item_to_read_2.deserialize(file);
    item_to_read_3.deserialize(file);
    item_to_read_4.deserialize(file, item_to_write_1.getSize());
    item_to_read_5.deserialize(file, item_to_write_1.getSize() + item_to_write_2.getSize());

    std::cout << "Item read 1:\n" << item_to_read_1.toString() << std::endl;
    std::cout << "Item read 2:\n" << item_to_read_2.toString() << std::endl;
    std::cout << "Item read 3:\n" << item_to_read_3.toString() << std::endl;
    std::cout << "Item read 4:\n" << item_to_read_4.toString() << std::endl;
    std::cout << "Item read 5:\n" << item_to_read_5.toString() << std::endl;

    file.close();

    return 0;
}