

#include "DataFile.h"

/***** STATIC CONSTANTS *****/

// uses ".dat" as default file extension.
const std::string DataFile::default_file_extension = ".dat";
const std::string DataFile::default_file_path = ".\\";
const char DataFile::hex_values_[16] =
    {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

/***** CONSTRUCTORS/DESTRUCTOR *****/

// Default constructor
//
// sets a blank filename and default file extension
// and creates a unique_ptr to an fstream
DataFile::DataFile():
    file_name_(""),
    file_extension_(default_file_extension),
    data_file_(std::make_unique<std::fstream>()),
    ios_openmode_(std::ios::binary) { }


DataFile::DataFile(std::string file_name, std::ios::openmode mode):
    data_file_(std::make_unique<std::fstream>()) {
    setFileName(file_name);
    open(file_name_, mode);
}

DataFile::DataFile(std::string file_name, std::string file_path, std::ios::openmode mode):
    data_file_(std::make_unique<std::fstream>()),
    file_path_(file_path) {
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
    
    ios_openmode_ = mode;
    
    // switch (mode) {
    //     case OpenMode::readonly:
    //         ios_openmode_ = std::ios::binary | std::ios::in;
    //         break;
    //     case OpenMode::overwrite:
    //         ios_openmode_ = std::ios::binary | std::ios::out; 
    //         break;
    //     case OpenMode::edit:
    //         ios_openmode_ = std::ios::binary | (std::ios::in | std::ios::out);
    //         break;
    // }
    
    data_file_->open(file_path_ + file_name_, ios_openmode_);

    // if file not opened (doesn't exist), use fstream to open file for writing,
    // which will create a new file if it doesn't already exist
    if (!data_file_->is_open()) {
        data_file_->open(file_name_, std::ios::binary | std::ios::out);
        data_file_->close();
        data_file_->open(file_path_ + file_name_, ios_openmode_);
    }

    if (!data_file_->is_open())
        throw std::ios_base::failure("Failed to open or create the file.");
}

void DataFile::open(std::string file_name, std::ios::openmode mode) {
    setFileName(file_name);
    open(mode);
}

void DataFile::open(std::string file_name, std::string file_path, std::ios::openmode mode) {
    setFileName(file_name);
    setFilePath(file_path);
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

std::string DataFile::getFilePath() const { return file_path_; }

int64_t DataFile::getFileSize() const {
    // check if file is open
    if (!isOpen())
        return -1;  // indicates error

    // save current position
    std::streampos curr_pos = data_file_->tellg();
    // move to end of file and save position as file size
    data_file_->seekg(0, std::ios::end);
    int64_t file_size = static_cast<int64_t>(data_file_->tellg());
    // move back to current position
    data_file_->seekg(curr_pos);

    // return end position, which is the file size in bytes
    return file_size;
}

// Returns the current std::ios::openmode for the file.
// If file is closed, returns the last openmode used.
std::ios_base::openmode DataFile::getOpenMode() const { return ios_openmode_; };

int64_t DataFile::getReadPos() const {
    // check if file is open, return -1 if it is not
    if (!isOpen())
        return -1;

    return static_cast<int64_t>(data_file_->tellg());
}

int64_t DataFile::getWritePos() const {
    // check if file is open, return -1 if it is not
    if (!isOpen())
        return -1;

    return static_cast<int64_t>(data_file_->tellp());
}

/***** SETTERS/MUTATORS *****/

void DataFile::setFileName(std::string file_name) {
    if (isOpen()) {
        throw std::runtime_error("File is already open. Cannot change file name at this time.");
    }
    size_t index = file_name.find_last_of('.');
    
    if (index == std::string::npos) {
        if (file_extension_.empty()) {
            setFileExtension(default_file_extension);
        }
        file_name_ = file_name + file_extension_;
    } else if (file_name.substr(index) != file_extension_) {
        file_extension_ = file_name.substr(index);
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

    size_t index = file_name_.find_last_of('.');
    if (index == std::string::npos) {
        file_name_ = file_name_ + file_extension_;
    } else {
        file_name_ = file_name_.substr(0, index) + file_extension_;
    }

}

void DataFile::setFilePath(std::string file_path) {
    file_path_ = file_path;
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

// Wrapper for std::fstream.is_open().
// Returns true if file is open.
bool DataFile::isOpen() const { return data_file_->is_open(); }

// Wrapper for std::fstream.eof().
// Returns true if eofbit is set.
bool DataFile::eof() const { return data_file_->eof(); }

// Wrapper for std::fstream.good().
// Returns true if no error flags are set.
bool DataFile::good() const { return data_file_->good(); }

// Wrapper for std::fstream.fail().
// Returns true if either badbit or failbit are set.
bool DataFile::fail() const { return data_file_->fail(); }

// Wrapper for std::fstream.bad().
// Returns true if badbit is set.
bool DataFile::bad() const { return data_file_->bad(); }

// Wrapper for std::fstream.clear().
// Resets the error state.
// Does not support setting specific error bits.
void DataFile::clear() { data_file_->clear(); }

/***** READ FUNCTIONS *****/


void DataFile::read(std::string &str) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");

    // check if at eof
    if (eof())
        throw std::out_of_range("End of file reached.");

    // read string length
    uint16_t len;
    read(&len);

    // create char buffer and read string into it
    std::vector<char> buffer(len + 1);
    readArray(buffer.data(), len);

    // verify that entire string was successfully read
    if (data_file_->gcount() != len)
        throw std::ios_base::failure("Failed to read the complete string data.");

    // add null terminator to char buffer
    buffer[len] = '\0';

    // convert to std::string
    str = std::string(buffer.data());
}

void DataFile::read(std::string &str, int64_t pos) {
    // move read pointer
    setReadPos(pos);
    // read from file
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
    // move write pointer
    setWritePos(pos);
    // write to file
    write(str);
}

// returns true if file is empty, false otherwise
bool DataFile::isEmpty() const { return getFileSize() == 0; }

// Prints out a hex dump to the console from data_file_ from start to end
// 
// Reads entire range at once; may not be suitable for large ranges.
void DataFile::hexDump(int64_t start, int64_t size) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open.");

    // calculate memory address of end of hex dump
    int64_t end = start + size;

    // initialize buffer and read the range in bytes
    std::vector<unsigned char> buffer(size);
    readArray(buffer.data(), size, start);

    int64_t address = start;                    // starting address
    int64_t index = 0;                          // buffer index
    int64_t lines = ((size - 1) / 16) + 1;      // number of lines of 16 bytes + 1 for partial line

    // print hex dump header
    // printf(" Address:    0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F\n");
    printf("\n\n");
    printf("    File:   %s\n", file_name_.c_str());
    printf("    Size:   %d bytes\n", getFileSize());
    if (end == 0) {
        return;
    }
    printf(" Address:   ");
    int hex_start = start % 16;
    for (int i = hex_start; i < hex_start + 8; ++i) {
        printf("%2c ", hex_values_[i % 16]);
    }
    printf(" ");
    for (int i = hex_start + 8; i < hex_start + 16; ++i) {
        printf("%2c ", hex_values_[i % 16]);
    }
    printf("\n================================================================================\n");

    // print the hex dump
    for (int line = 0; line < lines; ++line) {
        // print memory address in hex in an 8 wide column with leading 0's
        printf("|%08X| ", address);
        
        // loop through the next 8 bytes (if they exist) and print the bytes in
        // hex with a leading 0, or leave a blank space if that byte doens't exist
        for (int i = 0; i < 8; ++i) {
            if (address + i < end) {
                printf(" %02X", buffer[index + i]);
            } else {
                printf("   ");
            }
        }

        // add an extra space between first and last set of 8 bytes
        printf(" ");

        // loop through the next 8 bytes (if they exist) and print the bytes in
        // hex with a leading 0, or leave a blank space if that bye doens't exist
        for (int i = 8; i < 16; ++i) {
            if (address + i < end) {
                printf(" %02X", buffer[index + i]);
            } else {
                printf("   ");
            }
        }

        // print 2 spaces and a | to separate ascii representation
        printf("  |");

        // loop through the same 16 bytes (if they exist) as above, printing the
        // ascii character if it is printable, '.' if it is unprintable, or an empty
        // space if the byte doesn't exist
        unsigned char curr_byte = 0;
        for (int i = 0; i < 16; ++i) {
            if (address + i < end){
                curr_byte = buffer[index + i];
                if (curr_byte >= 32 && curr_byte <= 126) {
                    printf("%c", curr_byte);
                } else {
                    printf(".");
                }
            } else {
                printf(" ");
            }
        }

        // print | to close off ascii section and go to the next line
        printf("|\n");

        // increment address and index by one line
        address += 16;
        index += 16;
    }

    // print hex dump footer
    printf("================================================================================\n");
    // printf("    File: %s  Size: %d\n", file_name_.c_str(), getFileSize());
    printf("   Range:   0x%08X ~ 0x%08X\n", start, end - 1);
    printf("  Length:   %d bytes\n\n", size);
}

// Prints out a hex dump to the console from data_file_ of whole file
// 
// Reads entire file at once; may not be suitable for large file sizes.
void DataFile::hexDump() {
    hexDump(0, getFileSize());
}

