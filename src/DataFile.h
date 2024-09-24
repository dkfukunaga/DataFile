

#ifndef DATA_FILE_H
#define DATA_FILE_H

#include <fstream>
#include <memory>
#include <string>
#include <vector>

class DataFile {
public:
    // constructors/destructor
    DataFile();
    DataFile(std::string file_name);
    ~DataFile();

    // open/close functions
    void                            open();
    void                            open(std::string file_name);
    void                            openReadOnly();
    void                            openReadOnly(std::string file_name);
    void                            close();

    // getters/accessors
    std::string                     getFileName() const;
    std::string                     getFileExtension() const;
    long long                       getFileSize() const;
    bool                            isReadOnly() const;
    long long                       getReadPos() const;
    long long                       getWritePos() const;

    // setters/mutators
    void                            setFileName(std::string file_name);
    void                            setFileExtension(std::string extension);
    void                            setReadPos(long long pos);
    void                            setWritePos(long long pos);

    // fstream status
    bool                            isOpen() const;
    bool                            eof() const;
    bool                            good() const;
    bool                            fail() const;
    bool                            bad() const;
    void                            clear();

    // read functions
    template<typename T> void       read(T *data);
    template<typename T> void       read(T *data, long long pos);
    template<typename T> void       readArray(T *data, long long len);
    template<typename T> void       readArray(T *data, long long len, long long pos);
    void                            read(std::string &str);
    void                            read(std::string &str, long long pos);

    // write functions
    template<typename T> void       write(const T *data);
    template<typename T> void       write(const T *data, long long pos);
    template<typename T> void       writeArray(const T *data, long long len);
    template<typename T> void       writeArray(const T *data, long long len, long long pos);
    void                            write(const std::string &str);
    void                            write(const std::string &str, long long pos);

    // utility functions
    void                            hexDump();

private:
    // member variables
    std::unique_ptr<std::fstream>   data_file_;
    std::string                     file_name_;
    std::string                     file_extension_;
    bool                            read_only_;

    // static constants
    static const std::string        default_file_extension_;
};

/***** TEMPLATED READ FUNCTIONS *****/

template<typename T>
void DataFile::read(T *data) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open or could not be opened.");

    // check if read will go out of bounds
    std::streampos current_pos = data_file_->tellg();
    if (current_pos + static_cast<std::streamoff>(sizeof(T)) > getFileSize())
        throw std::out_of_range("End of file reached.");
    
    // read from file
    data_file_->read(reinterpret_cast<char*>(data), sizeof(T));
}

template<typename T>
void DataFile::read(T *data, long long pos) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open or could not be opened.");
    
    // move read pointer and read from file
    setReadPos(pos);
    read(data);
}

template<typename T>
void DataFile::readArray(T *data, long long len) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open or could not be opened.");

    // check if read will go out of bounds
    std::streampos current_pos = data_file_->tellg();
    if (current_pos + static_cast<std::streamoff>(len * sizeof(T)) > static_cast<std::streamoff>(getFileSize()))
        throw std::out_of_range("End of file reached.");
    
    // read from file    
    data_file_->read(reinterpret_cast<char*>(data), len * sizeof(T));
}

template<typename T>
void DataFile::readArray(T *data, long long len, long long pos) {
    setReadPos(pos);
    readArray(data, len);
}



/***** TEMPLATED WRITE FUNCTIONS *****/

template<typename T>
void DataFile::write(const T *data) {
    // write to file
    data_file_->write(reinterpret_cast<const char*>(data), sizeof(T));

    // Check for write errors
    if (data_file_->fail()) {
        throw std::ios_base::failure("Error occurred while writing to file.");
    }
}

template<typename T>
void DataFile::write(const T *data, long long pos) {
    // move write pointer and write to file
    setWritePos(pos);
    write(data);
}

template<typename T>
void DataFile::writeArray(const T *data, long long len) {
    // write(data, len * sizeof(T));
    data_file_->write(reinterpret_cast<const char*>(data), len * sizeof(T));
}

template<typename T>
void DataFile::writeArray(const T *data, long long len, long long pos) {
    setWritePos(pos);
    writeArray(data, len);
}


#endif


// maybe implement move constructor and overload operator=
// maybe implement swap()