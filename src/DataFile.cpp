

#include "DataFile.h"

/***** STATIC CONSTANTS *****/

const std::string DataFile::default_file_extension_ = ".dat";

/***** CONSTRUCTORS/DESTRUCTOR *****/

// Default constructor
//
// sets a blank filename and default file extension
// and creates a unique_ptr to an fstream
DataFile::DataFile():
    file_name_(""),
    file_extension_(default_file_extension_),
    data_file_(std::make_unique<std::fstream>()),
    ios_openmode(std::ios::binary) { };

DataFile::DataFile(std::string file_name, std::ios::openmode mode):
    data_file_(std::make_unique<std::fstream>()) {
    setFileName(file_name);
    open(file_name_, mode);
}

// Make sure file is closed upon destruction of DataFile object
DataFile::~DataFile() {
    close();
}

/***** OPEN/CLOSE FUNCTIONS *****/

void DataFile::open(std::ios::openmode mode) {
    if (file_name_.empty())
        return;
    
    ios_openmode = mode;
    
    // switch (mode) {
    //     case FileMode::readonly:
    //         ios_openmode = std::ios::binary | std::ios::in;
    //         break;
    //     case FileMode::overwrite:
    //         ios_openmode = std::ios::binary | std::ios::out; 
    //         break;
    //     case FileMode::edit:
    //         ios_openmode = std::ios::binary | (std::ios::in | std::ios::out);
    //         break;
    // }
    
    data_file_->open(file_name_, ios_openmode);

    // if file not opened (doesn't exist), use fstream to open file for writing,
    // which will create a new file if it doesn't already exist
    if (!data_file_->is_open()) {
        data_file_->open(file_name_, std::ios::binary | std::ios::out);
        data_file_->close();
        data_file_->open(file_name_, ios_openmode);
    }

    if (!data_file_->is_open())
        throw std::ios_base::failure("Failed to open or create the file.");
}

void DataFile::open(std::string file_name, std::ios::openmode mode) {
    setFileName(file_name);
    open(mode);
}

void DataFile::close() {
    if (data_file_->is_open()) {
        data_file_->close();
    }
}

/***** GETTERS/ACCESSORS *****/

std::string DataFile::getFileName() const { return file_name_; }

std::string DataFile::getFileExtension() const { return file_extension_.substr(1); }

int64_t DataFile::getFileSize() const {
    // check if file is open
    if (!isOpen())
        return -1;  // indicates error
    // save current position
    std::streampos curr_pos = data_file_->tellg();
    // move to end of file and save position
    data_file_->seekg(0, std::ios::end);
    std::streampos file_size = data_file_->tellg();
    // move back to current position
    data_file_->seekg(curr_pos);
    // return end position, which is the file size in bytes
    return static_cast<int64_t>(file_size);
}

std::ios_base::openmode DataFile::getOpenMode() const { return ios_openmode; };

int64_t DataFile::getReadPos() const {
    return static_cast<int64_t>(data_file_->tellg());
}

int64_t DataFile::getWritePos() const {
    return static_cast<int64_t>(data_file_->tellp());
}

/***** SETTERS/MUTATORS *****/

void DataFile::setFileName(std::string file_name) {
    if (isOpen()) {
        throw std::runtime_error("File is already open. Cannot change file name at this time.");
    }
    size_t idx;
    if (file_name[0] == '.') {
        idx = file_name.substr(1).find_last_of('.');
    } else {
        idx = file_name.find_last_of('.');
    }
    
    if (idx == std::string::npos) {
        if (file_extension_.empty()) {
            setFileExtension(default_file_extension_);
        }
        file_name_ = file_name + file_extension_;
    } else if (file_name.substr(idx) != file_extension_) {
        file_extension_ = file_name.substr(idx + 1);
        file_name_ = file_name;
    } else {
        file_name_ = file_name;
    }
}

void DataFile::setFileExtension(std::string extension) {
    if (extension[0] != '.') {
        file_extension_ = "." + extension;
    } else {
        file_extension_ = extension;
    }

    size_t idx;
    if (file_name_[0] == '.') {
        idx = file_name_.substr(1).find_last_of('.');
    } else {
        idx = file_name_.find_last_of('.');
    }
    if (idx == std::string::npos) {
        file_name_ = file_name_ + file_extension_;
    } else {
        file_name_ = file_name_.substr(0, idx + 1) + file_extension_;
    }

}

void DataFile::setReadPos(int64_t pos) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");

    // check if pos is out of bounds
    int64_t file_size = getFileSize();
    if ((pos < 0 && -pos > file_size) || (pos >= 0 && pos > file_size))
        throw std::out_of_range("Position is out of bounds.");

    // move pointer
    data_file_->seekg(pos, (pos < 0 ? std::ios::end : std::ios::beg));
}

void DataFile::setReadPosBegin() {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");
    
    data_file_->seekg(0, std::ios::beg);
}
void DataFile::setReadPosEnd() {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");
    
    data_file_->seekg(0, std::ios::end);
}

void DataFile::setWritePos(int64_t pos) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");

    // check if pos is out of bounds
    int64_t file_size = getFileSize();
    if ((pos < 0 && -pos > file_size) || (pos >= 0 && pos > file_size))
        throw std::out_of_range("Position is out of bounds.");

    // move pointer
    data_file_->seekp(pos, (pos < 0 ? std::ios::end : std::ios::beg));
}

void DataFile::setWritePosBegin() {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");
    
    data_file_->seekp(0, std::ios::beg);
}
void DataFile::setWritePosEnd() {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");
    
    data_file_->seekp(0, std::ios::end);
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

void DataFile::read(std::string &str, int64_t pos) {
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

void DataFile::write(const std::string &str, int64_t pos) {
    // setWritePos will verify file is open and pos is valid
    setWritePos(pos);
    write(str);
}

// Prints out a hex dump to the console from data_file_ from start to end
// 
// Reads entire range at once; may not be suitable for large file sizes.
void DataFile::hexDump(int64_t start, int64_t size) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");
    
    // int64_t size = end - start;           // number of bytes to display
    int64_t end = start + size;

    // initialize buffer and read the range in bytes
    // setReadPos(start);
    std::vector<unsigned char> buff(size);
    readArray(buff.data(), size, start);

    int64_t address = start;              // starting address
    int64_t index = 0;
    int64_t lines = ((size - 1) / 16) + 1;    // number of lines of 16 bytes + 1 for any extra

    // print hex dump header
    // printf(" Address:    0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F\n");
    printf("\n\n");
    printf("     File:  %s\n", file_name_.c_str());
    printf("     Size:  %d bytes\n", getFileSize());
    if (end == 0) {
        return;
    }
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
    // printf("    File: %s  Size: %d\n", file_name_.c_str(), getFileSize());
    printf("    Range:  0x%08X ~ 0x%08X\n", start, end);
    printf("   Length:  %d bytes\n\n", size);
}

// Prints out a hex dump to the console from data_file_ of whole file
// 
// Reads entire file at once; may not be suitable for large file sizes.
void DataFile::hexDump() {
    hexDump(0, getFileSize());
}

