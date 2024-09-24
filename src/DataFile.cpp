

#include "DataFile.h"

/***** STATIC CONSTANTS *****/

const std::string DataFile::default_file_extension_ = ".dat";

/***** CONSTRUCTORS/DESTRUCTOR *****/

// Default constructor
//
// sets a blank filename and creates a unique_ptr to an fstream
DataFile::DataFile():
    file_name_(""),
    file_extension_(default_file_extension_),
    data_file_(std::make_unique<std::fstream>()) { };

DataFile::DataFile(std::string file_name):
    data_file_(std::make_unique<std::fstream>()) {
    setFileName(file_name);
    open(file_name_);
}


DataFile::~DataFile() {
    close();
}

/***** OPEN/CLOSE FUNCTIONS *****/

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

/***** GETTERS/ACCESSORS *****/

std::string DataFile::getFileName() const { return file_name_; }

std::string DataFile::getFileExtension() const { return file_extension_.substr(1); }

long long DataFile::getFileSize() const {
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
    return static_cast<long long>(file_size);
}

long long DataFile::getReadPos() const {
    return static_cast<long long>(data_file_->tellg());
}

long long DataFile::getWritePos() const {
    return static_cast<long long>(data_file_->tellp());
}

/***** SETTERS/MUTATORS *****/

void DataFile::setFileName(std::string file_name) {
    if (isOpen()) {
        throw std::runtime_error("File is already open. Cannot change file name at this time.");
    }
    size_t idx = file_name.find_last_of('.');
    if (idx == std::string::npos) {
        if (file_extension_.empty()) {
            setFileExtension(default_file_extension_);
        }
        file_name_ = file_name + file_extension_;
    } else if (file_name.substr(idx) != file_extension_) {
        file_extension_ = file_name.substr(idx);
        file_name_ = file_name;
    } else {
        file_name_ = file_name;
    }
}

void DataFile::setFileExtension(std::string extension) {
    if (extension[0] != '.') {
        file_extension_ = "." + extension;
    }
    file_extension_ = extension;
}

void DataFile::setReadPos(long long pos) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");

    // check if pos is out of bounds
    long long file_size = getFileSize();
    if ((pos < 0 && -pos > file_size) || (pos >= 0 && pos > file_size))
        throw std::out_of_range("Position is out of bounds.");

    // move pointer
    data_file_->seekg(pos, (pos < 0 ? std::ios::end : std::ios::beg));
}

void DataFile::setWritePos(long long pos) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");

    // check if pos is out of bounds
    long long file_size = getFileSize();
    if ((pos < 0 && -pos > file_size) || (pos >= 0 && pos > file_size))
        throw std::out_of_range("Position is out of bounds.");

    // move pointer
    data_file_->seekp(pos, (pos < 0 ? std::ios::end : std::ios::beg));
}


/***** FILE STATUS/FLAGS *****/

bool DataFile::isOpen() const { return data_file_->is_open(); }

bool DataFile::eof() const { return data_file_->eof(); }

bool DataFile::good() const { return data_file_->good(); }

bool DataFile::fail() const { return data_file_->fail(); }

bool DataFile::bad() const { return data_file_->bad(); }

void DataFile::clear() { data_file_->clear(); }
/***** READ FUNCTIONS *****/

void DataFile::read(std::string &str) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");
    // check if at eof
    if (eof())
        throw std::out_of_range("End of file reached.");
    uint16_t len;
    read(&len);

    std::vector<char> buffer(len + 1);  // Use vector for safe memory allocation
    readArray(buffer.data(), len);

    // if (data_file_->gcount() != len)
    //     throw std::ios_base::failure("Failed to read the complete string data.");
    
    buffer[len] = '\0';  // Null-terminate
        
    str = std::string(buffer.data());
}

void DataFile::read(std::string &str, long long pos) {
    // setReadPos will verify file is open and pos is valid
    setReadPos(pos);
    read(str);
}

/***** WRITE FUNCTIONS *****/

void DataFile::write(const std::string &str) {
    // check string length
    size_t str_len = str.length();
    if (str_len > USHRT_MAX)
        throw std::length_error("String length exceeds maximum limit.");

    // write string length
    uint16_t len = static_cast<uint16_t>(str_len);
    write(&len);
    
    // Check for write errors after writing length
    if (data_file_->fail())
        throw std::ios_base::failure("Error occurred while writing to file.");
    
    // write string without null terminator
    data_file_->write(str.c_str(), len);

    // Check for write errors after writing string
    if (data_file_->fail())
        throw std::ios_base::failure("Error occurred while writing to file.");
}

void DataFile::write(const std::string &str, long long pos) {
    // setWritePos will verify file is open and pos is valid
    setWritePos(pos);
    write(str);
}

// Prints out a hex dump to the console from data_file_ from start to end
// 
// Reads entire range at once; may not be suitable for large file sizes.
void DataFile::hexDump(long long start, long long size) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");
    
    // long long size = end - start;           // number of bytes to display
    long long end = start + size;

    // initialize buffer and read the range in bytes
    // setReadPos(start);
    std::vector<unsigned char> buff(size);
    readArray(buff.data(), size, start);

    long long address = start;              // starting address
    long long index = 0;
    long long lines = ((size - 1) / 16) + 1;    // number of lines of 16 bytes + 1 for any extra

    // print hex dump header
    // printf(" Address:    0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F\n");
    printf("================================================================================\n");

    // print the hex dump
    for (int line = 0; line < lines; ++line) {
        // print memory address in hex in an 8 wide column with leading 0's
        printf("|%08X| ", address);
        
        // loop through the next 8 bytes (if they exist) and print the bytes in
        // hex with a leading 0, or leave a blank space if that bye doens't exist
        for (int i = 0; i < 8; ++i) {
            if (address + i < end) {
                printf(" %02X", buff[index + i]);
            } else {
                printf("   ");
            }
        }

        // add a space between first 8 bytes
        printf(" ");

        // loop through the next 8 bytes (if they exist) and print the bytes in
        // hex with a leading 0, or leave a blank space if that bye doens't exist
        for (int i = 8; i < 16; ++i) {
            if (address + i < end) {
                printf(" %02X", buff[index + i]);
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
            if (address + i < end){
                if (buff[index + i] >= 32 && buff[index + i] <= 126) {
                    printf("%c", buff[index + i]);
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
        index += 16;
    }

    // print hex dump footer
    printf("================================================================================\n");
    printf("  Size: %d bytes\n\n", size);
}

// Prints out a hex dump to the console from data_file_ of whole file
// 
// Reads entire file at once; may not be suitable for large file sizes.
void DataFile::hexDump() {
    hexDump(0, getFileSize());
}

