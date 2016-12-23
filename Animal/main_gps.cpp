#include <iostream>
#include <pthread.h>
#include "defines.h"
#include "threadsanimal.h"

using namespace std;

int main()
{
  CThreadsAnimal animal;

  animal.run();
  //
  // animal = new CThreadsAnimal();
  //
  // animal->run();

  pthread_exit(0);

  return 0;
}
