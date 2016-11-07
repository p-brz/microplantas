#include "Buffer.h"

bool Buffer::put(const void * data, int length){
    if(putAt(data, length, idx)){
        this->idx += length;
        return true;
    }
    return false;
}

bool Buffer::putAt(const void * data, int length, int idx){
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

bool Buffer::get(void * data, int length){
    byte * bytes = (byte *)data;

    if(length + idx > BUF_SIZE){
        return false;
    }

    for(int i = 0; i < idx; ++i){
        bytes[i] = buffer[idx];
    }
    return true;
}
