
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "..\src\DataFile.h"
#include "testItem.cpp"
#include <sstream>



TEST_CASE("Verify DataFile functions") {

    std::string str_1("Hello world!");
    std::string str_2("This is the song that doesn't end, yes it goes on and on my "
                      "friend! Some people started singing it not knowing what it "
                      "was, and they'll continue singing it forever just because...");

    TestItem test_item_1(str_1, 0xEFCDAB8967452301, 420.69);
    TestItem test_item_2(str_2, 0x1032547698BADCFE, 69.420);

    std::string file_name(".\\doctest\\data\\test_name");

    DataFile file(file_name, FileMode::overwrite);
    file.close();

    SUBCASE("verify FileModes") {
        std::ios::openmode file_mode = std::ios::binary | std::ios::in | std::ios::out;
        file.open(FileMode::edit);
        CHECK(file.getOpenMode() == file_mode);
        file.close();

        file_mode = std::ios::binary | std::ios::in;
        file.open(FileMode::readonly);
        CHECK(file.getOpenMode() == file_mode);
        file.close();

        file_mode = std::ios::binary | std::ios::out;
        file.open(FileMode::overwrite);
        CHECK(file.getOpenMode() == file_mode);
        file.close();
    }

    SUBCASE("verify getters and setters") {
        file.open(FileMode::readonly);
        file.hexDump();

        CHECK(file.isOpen());
        CHECK(file.getFileName() == file_name + ".dat");
        CHECK(file.getFileExtension() == "dat");
        CHECK(file.getFileSize() == 0);

        file.close();

        CHECK_FALSE(file.isOpen());

        file.setFileName(".\\doctest\\data\\test_file.dat");
        file.setFileExtension("dt2");
        file.open(FileMode::overwrite);

        CHECK(file.getFileName() == ".\\doctest\\data\\test_file.dt2");
        CHECK(file.getFileExtension() == "dt2");

        test_item_1.serialize(file);
        file.close();
        file.open();
        file.hexDump();

        file.setReadPos(7);
        CHECK(file.getReadPos() == 7);

        file.setWritePos(13);
        CHECK(file.getWritePos() == 13);

        file.setReadPosBegin();
        CHECK(file.getReadPos() == 0);

        file.setWritePosBegin();
        CHECK(file.getWritePos() == 0);

        file.setReadPosEnd();
        CHECK(file.getReadPos() == test_item_1.getSize());

        file.setWritePosEnd();
        CHECK(file.getWritePos() == test_item_1.getSize());

        file.close();
    }

    SUBCASE("verify file writes") {

        file.open();
        test_item_1.serialize(file);

        short num = -1;
        file.setWritePosBegin();
        file.write(&num, test_item_1.getSize());
        file.hexDump();
        file.close();

        std::fstream in_file;
        in_file.open(".\\doctest\\data\\test_name.dat", std::ios::binary | std::ios::in);

        TestItem read_item;
        unsigned short str_len;

        in_file.read(reinterpret_cast<char *>(&read_item.test_id), sizeof(read_item.test_id));
        in_file.read(reinterpret_cast<char *>(&str_len), sizeof(str_len));
        char test_str[str_len + 1];
        in_file.read(test_str, str_len);
        test_str[str_len] = '\0';
        read_item.test_str = std::string(test_str);
        in_file.read(reinterpret_cast<char *>(&read_item.test_long), sizeof(read_item.test_long));
        in_file.read(reinterpret_cast<char *>(&read_item.test_float), sizeof(read_item.test_float));
        in_file.read(reinterpret_cast<char *>(read_item.test_foot), read_item.foot_len * sizeof(read_item.test_foot[0]));

        CHECK(read_item.test_id == test_item_1.test_id);
        CHECK(read_item.test_str == test_item_1.test_str);
        CHECK(read_item.test_long == test_item_1.test_long);
        CHECK(read_item.test_float == doctest::Approx(test_item_1.test_float));
        CHECK(read_item.test_foot[0] == test_item_1.test_foot[0]);
        CHECK(read_item.test_foot[1] == test_item_1.test_foot[1]);
        CHECK(read_item.test_foot[2] == test_item_1.test_foot[2]);
        CHECK(read_item.test_foot[3] == test_item_1.test_foot[3]);
        CHECK(read_item.test_foot[4] == test_item_1.test_foot[4]);
        CHECK(read_item.test_foot[5] == test_item_1.test_foot[5]);
        CHECK(read_item.getSize() == test_item_1.getSize());

        short check_num;
        in_file.read(reinterpret_cast<char *>(&check_num), sizeof(check_num));
        CHECK(check_num == num);

        auto file_end = in_file.tellg();
        CHECK(read_item.getSize() + sizeof(num) == static_cast<int>(file_end));

        in_file.close();
    }

    SUBCASE("verify file reads") {

        file.open();
        test_item_2.serialize(file);
        file.hexDump();
        file.close();

        TestItem read_item;

        file.open(FileMode::readonly);

        read_item.deserialize(file);

        CHECK(read_item.test_id == test_item_2.test_id);
        CHECK(read_item.test_str == test_item_2.test_str);
        CHECK(read_item.test_long == test_item_2.test_long);
        CHECK(read_item.test_float == doctest::Approx(test_item_2.test_float));
        CHECK(read_item.test_foot[0] == test_item_2.test_foot[0]);
        CHECK(read_item.test_foot[1] == test_item_2.test_foot[1]);
        CHECK(read_item.test_foot[2] == test_item_2.test_foot[2]);
        CHECK(read_item.test_foot[3] == test_item_2.test_foot[3]);
        CHECK(read_item.test_foot[4] == test_item_2.test_foot[4]);
        CHECK(read_item.test_foot[5] == test_item_2.test_foot[5]);
        CHECK(read_item.getSize() == test_item_2.getSize());
        CHECK(read_item.getSize() == file.getFileSize());

        file.close();
    }

    file.close();
}

