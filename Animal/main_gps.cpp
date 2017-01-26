#include "defines.h"
//#include "threadsanimal.h"
#include "gps.h"

using namespace std;

int main()
{
  CGps gps;

  gps.initGps();

  while (1) {
    gps.readGps();
    delay(1000);
  }
//   CThreadsAnimal animal;
//
//   animal.run();
//   //
//   // animal = new CThreadsAnimal();
//   //
//   // animal->run();
//
//   pthread_exit(0);

  return 0;
}
