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

#ifndef MyClass_h
#define MyClass_h

#include <Arduino.h>
#include <StensTimer.h>

/* To allow callbacks on class instances you should let your class implement
IStensTimerListener and implement its timerCallback function as shown below */
class MyClass : public IStensTimerListener {

    public:
      MyClass();
      void setName(String name);

      void timerCallback(Timer* timer); // You must implement this

    private:
      String _name = "Unknown";
  };

#endif
