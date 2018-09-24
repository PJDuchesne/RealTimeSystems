#ifndef ZooKeeper_H
#define ZooKeeper_H

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
-> Name:  ZooKeeper.h
-> Date: Sept 20, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

// Using raw string literals to declare multiline strings
// Intentional empty line at start and end

// Array is located at the end of the file

#include <string>

extern const std::string aardvark;
extern const std::string beaver;
extern const std::string cow;
extern const std::string dog;
extern const std::string elephant;
extern const std::string frog;
extern const std::string gorilla;
extern const std::string horse;
// extern const std::string insect;
// extern const std::string jester;
extern const std::string kangaroo;
extern const std::string lizard;
extern const std::string moose;
// extern const std::string ninja;
extern const std::string octopus;
extern const std::string parrot;
// extern const std::string quarter;
extern const std::string rabbit;
extern const std::string scorpion;
extern const std::string turtle;
extern const std::string unicorn;
extern const std::string vulture;
extern const std::string wolf;
// extern const std::string xwing;
// extern const std::string yoda;
extern const std::string zebra;

#define ZOO_SIZE 20

const std::string zoo[ZOO_SIZE] = {
  aardvark,
  beaver,
  cow,
  dog,
  elephant,
  frog,
  gorilla,
  horse,
  // insect,
  // jester,
  kangaroo,
  lizard,
  moose,
  // ninja,
  octopus,
  parrot,
  // quarter,
  rabbit,
  scorpion,
  turtle,
  unicorn,
  vulture,
  wolf,
  // xwing,
  // yoda,
  zebra
};

#endif /* ZooKeeper_H */
