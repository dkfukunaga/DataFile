

#include "DataFile.h"

const std::string DataFile::default_file_extension_ = ".dat";


DataFile::DataFile():
    file_name_(""),
    data_file_(std::make_unique<std::fstream>()) { };

DataFile::DataFile(std::string file_name):
    data_file_(std::make_unique<std::fstream>()) {
    setFileName(file_name);
    open(file_name_);
}


DataFile::~DataFile() {
    close();
}


std::string DataFile::getFileName() const { return file_name_; }

std::string DataFile::getFileExtension() const { return file_extension_.substr(1); }

void DataFile::setFileName(std::string file_name) {
    int len = file_name.length();
    if (len > 4 && file_name.substr(len - 4) == file_extension_)
        file_name_ = file_name; 
    else
        file_name_ = file_name + file_extension_;
}

void DataFile::setFileExtension(std::string extension) {
    file_extension_ = "." + extension;
}

bool DataFile::isOpen() const { return data_file_->is_open(); }

int DataFile::getFileSize() const {
    // check if file is open
    if (!isOpen())
        return -1;  // indicates error
    // save current position
    auto curr_pos = data_file_->tellg();
    // move to end of file and save position
    data_file_->seekg(0, std::ios::end);
    auto file_size = data_file_->tellg();
    // move back to current position
    data_file_->seekg(curr_pos);
    // return end position, which is the file size in bytes
    return static_cast<int>(file_size);
}

bool DataFile::eof() const { return data_file_->eof(); }

bool DataFile::good() const { return data_file_->good(); }

bool DataFile::fail() const { return data_file_->fail(); }

bool DataFile::bad() const { return data_file_->bad(); }

void DataFile::clear() { data_file_->clear(); }

void DataFile::open() {
    if (file_name_.empty())
        return;
    
    data_file_->open(file_name_, std::ios::binary | std::ios::in | std::ios::out);

    // if file not opened (doesn't exist), use fstream to open file for writing,
    // which will create a new file if it doesn't already exist
    if (!data_file_->is_open()) {
        data_file_->open(file_name_, std::ios::binary | std::ios::out);
        data_file_->close();
        data_file_->open(file_name_, std::ios::binary | std::ios::in | std::ios::out);
    }

    if (!data_file_->is_open())
        throw std::ios_base::failure("Failed to open or create the file.");
}

void DataFile::open(std::string file_name) {
    setFileName(file_name);
    open();
}

void DataFile::close() {
    if (data_file_->is_open()) {
        data_file_->close();
    }
}


void DataFile::moveReadPointer(long long pos) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open or could not be opened.");

    // check if pos is out of bounds
    long long file_size = getFileSize();
    if ((pos < 0 && -pos > file_size) || (pos >= 0 && pos > file_size))
        throw std::out_of_range("Position is out of bounds.");

    // move pointer
    data_file_->seekg(pos, (pos < 0 ? std::ios::end : std::ios::beg));
}

void DataFile::moveWritePointer(long long pos) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open or could not be opened.");

    // check if pos is out of bounds
    long long file_size = getFileSize();
    if ((pos < 0 && -pos > file_size) || (pos >= 0 && pos > file_size))
        throw std::out_of_range("Position is out of bounds.");

    // move pointer
    data_file_->seekp(pos, (pos < 0 ? std::ios::end : std::ios::beg));
}


void DataFile::read(std::string *str) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open or could not be opened.");
    // check if at eof
    if (eof())
        throw std::out_of_range("End of file reached.");
    unsigned short len;
    read(&len, 2);

    std::vector<char> buffer(len + 1);  // Use vector for safe memory allocation
    data_file_->read(buffer.data(), len);

    if (data_file_->gcount() != len)
        throw std::ios_base::failure("Failed to read the complete string data.");
    
    buffer[len] = '\0';  // Null-terminate
        
    *str = std::string(buffer.data());
}

void DataFile::read(std::string *str, long long pos) {
    // moveReadPointer will verify file is open and pos is valid
    moveReadPointer(pos);
    read(str);
}

void DataFile::write(const std::string &str) {
    // check string length
    size_t str_len = str.length();
    if (str_len > USHRT_MAX)
        throw std::length_error("String length exceeds maximum limit.");

    // write string length
    unsigned short len = static_cast<unsigned short>(str_len);
    write(&len, 2);
    
    // Check for write errors after writing length
    if (data_file_->fail())
        throw std::ios_base::failure("Error occurred while writing to file.");
    
    // write string without null terminator
    write(str.c_str(), len);

    // Check for write errors after writing string
    if (data_file_->fail())
        throw std::ios_base::failure("Error occurred while writing to file.");
}

void DataFile::write(const std::string &str, long long pos) {
    // moveWritePointer will verify file is open and pos is valid
    moveWritePointer(pos);
    write(str);
}



