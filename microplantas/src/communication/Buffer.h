#ifndef BUFFER_H
#define BUFFER_H

#define BUF_SIZE 64

class Buffer{
public:
    using byte = unsigned char;

public:
    Buffer() : idx(0)
    {}

public:
    bool put(const void * data, int length);
    bool putAt(const void * data, int length, int idx);
    bool get(void * data, int length);

public:
    void reset(){
        this->forward(0);
        this->_size = 0;
    }
    void forward(int idx){
        this->idx = idx;
    }
    int index() const{
        return idx;
    }

public:
    const byte * data() const{
        return buffer;
    }
    byte * data(){
        return buffer;
    }
    int size() const{
        return _size;
    }
    void size(int length){
        this->_size = length;
    }
    int capacity() const{
        return BUF_SIZE;
    }

private:
    byte buffer[BUF_SIZE];
    int idx; //aponta onde sera lido/escrito
    int _size;
};

#endif // BUFFER_H
