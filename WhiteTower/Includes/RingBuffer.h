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

// For std::unique_ptr
#include <memory>
#include <iostream>

template <class T>
class RingBuffer {
  public:

    RingBuffer(int size) : buffer_(std::unique_ptr<T[]>(new T[size])),
                           buffer_size_(size),
                           full_(false),
                           empty_(true),
                           front_(0),
                           back_(0) {}

    void Add(T data);
    T Get();
    void GetAll(T data[]); // TODO: Gets all data and returns it as an array 
    void Reset(); // Not actually sure what this will be used for, but it seems useful to have
    bool Empty() const;
    bool Full() const;

  private:
    std::unique_ptr<T[]> buffer_;
    const int buffer_size_; // Should not be changed after initialization
    int front_; // Where data is put in (points to next position to place data)
    int back_;  // Where data is removed
    bool full_;
    bool empty_;
           
};

#include "../RingBuffer.tpp"

#endif /* RingBuffer_H */
