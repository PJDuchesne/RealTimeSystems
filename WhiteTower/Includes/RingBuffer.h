#ifndef RingBuffer_H
#define RingBuffer_H

/*
__/\\\\\\\\\\\\\_____/\\\\\\\\\\\__/\\\\\\\\\\\\____        
 _\/\\\/////////\\\__\/////\\\///__\/\\\////////\\\__       
  _\/\\\_______\/\\\______\/\\\_____\/\\\______\//\\\_      
   _\/\\\\\\\\\\\\\/_______\/\\\_____\/\\\_______\/\\\_     
    _\/\\\/////////_________\/\\\_____\/\\\_______\/\\\_    
     _\/\\\__________________\/\\\_____\/\\\_______\/\\\_   
      _\/\\\___________/\\\___\/\\\_____\/\\\_______/\\\__  
       _\/\\\__________\//\\\\\\\\\______\/\\\\\\\\\\\\/___
        _\///____________\/////////_______\////////////_____
-> Name:  RingBuffer.h
-> Date: Sept 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

template <class T>
class RingBuffer {
  private:
    T *buffer_;
    const int buffer_size_; // Should not be changed after initialization
    int front_; // Where data is put in (points to next position to place data)
    int back_;  // Where data is removed

  public:

    RingBuffer(int size) : buffer_(new T[size]),
                           buffer_size_(size),
                           front_(0),
                           back_(0) {}

    ~RingBuffer() { delete[] buffer_; }                    

    void Add(T data);
    T Get();
    void Pop();
    void Reset();
    bool Empty() const;
    bool Full() const;

};

// Templates are typically implemented in the header file,
// or separated into a ".tpp" file for readability.
#include "../RingBuffer.tpp"

#endif /* RingBuffer_H */
