/*
Copyright (C) 2017  Arjen Stens

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <MyClass.h>

#define SHOW_NAME_ACTION 1

MyClass::MyClass(){
  // Setup stuff
}

/* This function will be called whenever a timer is due,
the set timer is passed as a parameter so you can read any property you need. */
void MyClass::timerCallback(Timer* timer){

  /* Check if timer meets certain requirement(s) */
  if(timer->getAction() == SHOW_NAME_ACTION){
    /* Show name to serial monitor */
    Serial.println(_name);

    /* Since StensTimer is a singleton you can access the same StensTimer
     instance that was used in the main class from anywhere.
     Here we create a new timer for this classes instance with action '123' */
    StensTimer::getInstance()->setTimer(this, 123, 2000);
  }
  else if(timer->getAction() == 123){
    Serial.print("Hi! It's ");
    Serial.print(_name);
    Serial.println(" again!");
  }

}

/* Simple setter for this classes _name */
void MyClass::setName(String name){
  _name = name;
}
