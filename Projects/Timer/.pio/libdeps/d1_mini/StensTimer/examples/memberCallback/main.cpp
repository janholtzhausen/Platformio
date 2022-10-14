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

#include <Arduino.h>
#include <MyClass.h>
#include <StensTimer.h>

/* stensTimer variable to be used later in the code */
StensTimer* stensTimer;

/* Define some custom Action code */
#define SHOW_NAME_ACTION 1

void setup() {
  Serial.begin(9600);

  /* Save instance of StensTimer to the stensTimer variable */
  stensTimer = StensTimer::getInstance();


  /* Example of two different instances of a class and set their names */
  MyClass* myClassOne = new MyClass();
  MyClass* myClassTwo = new MyClass();
  myClassOne->setName("One");
  myClassTwo->setName("Two");

  /* Set timers for myClassOne and myClassTwo to show their name */
  stensTimer->setTimer(myClassOne, SHOW_NAME_ACTION, 1000);
  stensTimer->setTimer(myClassTwo, SHOW_NAME_ACTION, 2000);

  Serial.println("Setup completed...");
}

void loop() {
  /* Let StensTimer do it's magic every time loop() is executed */
  stensTimer->run();
}
