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

template <class T>
void RingBuffer<T>::Add(T data) {
  if (full_) {
    std::cout << "[RingBuffer::Add()] WARNING: BUFFER IS FULL, DATA WILL BE DISCARDED\n";
  }
  else {

    buffer_[front_] = data;

    // Increment front_
    front_ = (front_ + 1) % buffer_size_;

    // Buffer is full if we add data and front_ == back_
    full_ = (front_ == back_);
    empty_ = false;

  }
}

template <class T>
T RingBuffer<T>::Get() {

  // If empty, returns an unitialized value of type T (i.e. 0 for int)
  if (Empty()) return T();

  T return_val = buffer_[back_];
  // Decrement 'back_'
  back_ = (back_ + 1) % buffer_size_;

  // The buffer is empty if the front and back are equal
  // Note: Provided the queue is not full (which it can't be at this point)
  full_ = false;
  empty_ = (front_ == back_);

  return return_val;
}

  // Decrement front to erase latest data
template <class T>
void RingBuffer<T>::Pop() {
  if (Empty()) return;
  full_ = false;
  front_ = (front_ + buffer_size_ - 1) % buffer_size_;
  empty_ = (front_ == back_);
}

template <class T>
void RingBuffer<T>::Reset() {
  front_ = 0;
  back_ = 0;
  full_ = false;
  empty_ = true;
}

template <class T>
bool RingBuffer<T>::Empty() const {
  return empty_;
}

template <class T>
bool RingBuffer<T>::Full() const {
  return full_;
}
