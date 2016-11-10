#ifndef BUFFER_H
#define BUFFER_H

#define BUF_SIZE 64

template<unsigned CAPACITY>
class GenericBuffer{
public:
    using byte = unsigned char;

public:
    GenericBuffer() : idx(0)
    {}

public:
    bool put(byte value){
        return put(&value, 1);
    }
    bool put(const void * data, int length){
        if(putAt(data, length, idx)){
            this->idx += length;
            return true;
        }
        return false;
    }
    bool putAt(const void * data, int length, int idx){
        const byte * bytes = (const byte *)data;

        if(length + idx > capacity() || idx < 0){
            return false;
        }

        for(int i = 0; i < length; ++i){
            buffer[idx] = bytes[i];
            ++idx;
        }

        if(idx > _size){
            _size = idx;
        }

        return true;
    }
    bool get(void * data, int length){
        byte * bytes = (byte *)data;

        if(length + idx > CAPACITY){
            return false;
        }

        for(int i = 0; i < length; ++i){
            bytes[i] = buffer[idx];
            ++idx;
        }
        return true;
    }

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
        return CAPACITY;
    }
    int available() const{
        return size() - index();
    }

private:
    byte buffer[CAPACITY];
    int idx; //aponta onde sera lido/escrito
    int _size;
};

using Buffer = GenericBuffer<BUF_SIZE>;

#endif // BUFFER_H
