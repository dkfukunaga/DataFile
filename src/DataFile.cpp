

#include "DataFile.h"

/***** STATIC CONSTANTS *****/

const std::string DataFile::default_file_extension_ = ".dat";

/***** CONSTRUCTORS/DESTRUCTOR *****/

// Default constructor
//
// sets a blank filename and creates a unique_ptr to an fstream
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
    int len = file_name.length();
    if (len > 4 && file_name.substr(len - 4) == file_extension_)
        file_name_ = file_name; 
    else
        file_name_ = file_name + file_extension_;
}

void DataFile::setFileExtension(std::string extension) {
    file_extension_ = "." + extension;
}

void DataFile::setReadPos(long long pos) {
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

void DataFile::setWritePos(long long pos) {
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
        throw std::runtime_error("File is not open or could not be opened.");
    // check if at eof
    if (eof())
        throw std::out_of_range("End of file reached.");
    unsigned short len;
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
    unsigned short len = static_cast<unsigned short>(str_len);
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



