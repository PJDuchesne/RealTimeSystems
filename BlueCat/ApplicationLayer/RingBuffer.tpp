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
-> Name:  RingBuffer.tpp (template pp file, because templates)
-> Date: Sept 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

/*
    Function: Add
    Input:  data: Data to add to the ring buffer
    Brief: Adds the provided data to the ring buffer at the front position and then increments it
*/
template <class T>
void RingBuffer<T>::Add(T data) {
    if (!Full()) {
        // Add data and increment front_
        buffer_[front_] = data;
        front_ = (front_ + 1) % buffer_size_;
    }
}

/*
    Function: Add
    Ouytput: return_val: Data that was retrieved from the ring buffer
    Brief: Retrieves data from the end of the buffer and increments the back position
*/
template <class T>
T RingBuffer<T>::Get() {

    // If empty, returns an unitialized value of type T (i.e. 0 for int)
    if (Empty()) return T();

    T return_val = buffer_[back_];
    // Decrement 'back_'
    back_ = (back_ + 1) % buffer_size_;

    return return_val;
}

/*
    Function: Pop
    Brief: Increments the front of the buffer to delete the latest value stored
           (This is used in the Monitor with the delete/backspace input)
*/
template <class T>
void RingBuffer<T>::Pop() {
    if (Empty()) return;
    front_ = (front_ + buffer_size_ - 1) % buffer_size_;
}

/*
    Function: Reset
    Brief: Resets the buffer by zeroing both the front and the back
           (This is not used in the code but is useful to have inthe template function for future use)
*/
template <class T>
void RingBuffer<T>::Reset() {
    front_ = 0;
    back_ = 0;
}

/*
    Function: Empty
    Output: Boolean: True if the buffer is empty, false if it is not
    Brief: Checks if the buffer is current empty and returns a boolean accordingly
*/
template <class T>
bool RingBuffer<T>::Empty() const {
    return (front_ == back_);
}

/*
    Function: Full
    Output: Boolean: True if the buffer is full, false if it is not
    Brief: Checks if the buffer is current full and returns a boolean accordingly
*/
template <class T>
bool RingBuffer<T>::Full() const {
    return ((front_ + 1) % buffer_size_ == back_);
}
