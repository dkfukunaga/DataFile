

#ifndef DATA_FILE_H
#define DATA_FILE_H

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class DataFile {
public:
    DataFile();
    DataFile(std::string file_name);
    ~DataFile();

    void                            open();
    void                            open(std::string file_name);
    void                            close();

    std::string                     getFileName() const;
    std::string                     getFileExtension() const;
    int                             getFileSize() const;

    void                            setFileName(std::string file_name);
    void                            setFileExtension(std::string extension);

    bool                            isOpen() const;
    bool                            eof() const;
    bool                            good() const;
    bool                            fail() const;
    bool                            bad() const;
    void                            clear();

    void                            moveReadPointer(long long pos);
    void                            moveWritePointer(long long pos);

    void                            read(std::string *str);
    void                            read(std::string *str, long long pos);
    void                            write(const std::string &str);
    void                            write(const std::string &str, long long pos);

    template<typename T>            void read(T *data);
    template<typename T>            void read(T *data, long long pos);
    template<typename T, size_t N>  void read(const T (&arr)[N]);
    template<typename T, size_t N>  void read(const T (&arr)[N], long long pos);

    template<typename T>            void write(const T *data);
    template<typename T>            void write(const T *data, long long pos);
    template<typename T, size_t N>  void write(const T (&arr)[N]);
    template<typename T, size_t N>  void write(const T (&arr)[N], long long pos);

private:
    std::unique_ptr<std::fstream>   data_file_;
    std::string                     file_name_;
    std::string                     file_extension_;

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
    moveReadPointer(pos);
    read(data);
}


template<typename T, size_t N>
void DataFile::read(const T (&arr)[N]) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open or could not be opened.");

    // check if at eof
    std::streamoff current_pos = data_file_->tellg();
    if (current_pos + static_cast<std::streamoff>(N * sizeof(T)) > getFileSize())
        throw std::out_of_range("End of file reached.");
    
    // read from file
    data_file_->read(reinterpret_cast<char*>(arr), N * sizeof(T));
}

template<typename T, size_t N>
void DataFile::read(const T (&arr)[N], long long pos) {
    // check if file is open
    if (!isOpen())
        throw std::runtime_error("File is not open or could not be opened.");

    // check if at eof
    if (pos + (sizeof(T) * N) > getFileSize())
        throw std::out_of_range("End of file reached.");
    
    // move read pointer and read from file
    moveReadPointer(pos);
    data_file_->read(reinterpret_cast<char*>(arr), N * sizeof(T));
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
    moveWritePointer(pos);
    write(data);
}


template<typename T, size_t N>
void DataFile::write(const T (&arr)[N]) {
    // write to file
    data_file_->write(reinterpret_cast<const char*>(arr), N * sizeof(T));

    // Check for write errors
    if (data_file_->fail()) {
        throw std::ios_base::failure("Error occurred while writing to file.");
    }
}

template<typename T, size_t N>
void DataFile::write(const T (&arr)[N], long long pos) {
    // move write pointer and write to file
    moveWritePointer(pos);
    write(arr);
}


#endif


// maybe implement move constructor and overload operator=
// maybe implement swap()