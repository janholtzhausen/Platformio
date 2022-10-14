#include <Arduino.h>
#include <StensTimer.h>

/* stensTimer variable to be used later in the code */
StensTimer* stensTimer;

/* define some custom Action codes */
#define DO_THING_ONE 1
#define DO_THING_TWO 2
#define DO_THING_THREE 3

/* this function will be called whenever a timer is due,
the set timer is passed as a parameter so you can read any property you need. */
void timerCallback(Timer* timer){
  /* Print some values of this timer */
  Serial.print("Timer call -> Action: ");
  Serial.print(timer->getAction());
  Serial.print(", Current Time: ");
  Serial.println(millis());
}

void setup() {
  Serial.begin(9600);

  /* Save instance of StensTimer to the tensTimer variable*/
  stensTimer = StensTimer::getInstance();

  /* Tell StensTimer which callback function to use */
  stensTimer->setStaticCallback(timerCallback);

  /* perform thing_one after 2 seconds */
  stensTimer->setTimer(DO_THING_ONE, 2000);
  /* perform thing_two two times with 4 seconds in between */
  stensTimer->setTimer(DO_THING_TWO, 4000, 2);

  /* perform thing_three every 6 seconds indefiniteley,
  you the timer creation functions return the timer's instance so you can save it */
  Timer* timerThree = stensTimer->setInterval(DO_THING_THREE, 6000);

  /* you can always delete a timer, uncomment to test */
  // stensTimer->deleteTimers(); // deletes all _timers
  // stensTimer->deleteTimer(timerThree); // deletes timerThree

  Serial.println("Setup completed...");
}

void loop() {
  /* let StensTimer do it's magic every time loop() is executed */
  stensTimer->run();
}
