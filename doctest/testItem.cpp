

#include "testItem.h"



TestItem::TestItem() { };

TestItem::TestItem(std::string new_str, long long new_long, float new_float):
    test_str(new_str),
    test_long(new_long),
    test_float(new_float) { };


void TestItem::serialize(DataFile &file){
    file.write(&test_id);
    file.write(test_str);
    file.write(&test_long);
    file.write(&test_float);
    file.writeArray(test_foot, foot_len);
}

void TestItem::serialize(DataFile &file, long long pos) {
    file.setWritePos(pos);
    serialize(file);
}

void TestItem::deserialize(DataFile &file) {
    file.read(&test_id);
    file.read(test_str);
    file.read(&test_long);
    file.read(&test_float);
    file.readArray(test_foot, foot_len);
}

void TestItem::deserialize(DataFile &file, long long pos) {
    file.setReadPos(pos);
    deserialize(file);
}

int TestItem::getSize() {
    return sizeof(test_id) + 2 + test_str.length() + sizeof(test_long) +
           sizeof(test_float) + sizeof(test_foot);
}


std::string TestItem::toString() {
    std::stringstream footer_str;
    footer_str << "[0x";
    footer_str << std::hex << std::uppercase << std::setfill('0') << std::setw(4);
    footer_str << test_foot[0];

    for (int i = 1; i < foot_len; ++i) {
        footer_str << ", 0x" << std::hex << std::setfill ('0') << std::setw(4) << test_foot[i];
    }
    footer_str <<"]";

    std::stringstream ss;

    ss << std::hex << std::uppercase << std::setfill('0')
       << "Test ID:     0x" << std::setw(4) << _byteswap_ulong(test_id) << "\n"
       << "Test String: \"" << test_str << "\"\n"
       << "Test Long:   0x" << std::setw(16) << _byteswap_uint64(test_long) << "\n"
       << "Test Float:  " << std::to_string(test_float) << "\n"
       << "Test Footer: " << footer_str.str() << "\n\n";
    
    return ss.str();
} 