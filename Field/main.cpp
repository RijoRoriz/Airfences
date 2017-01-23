#include "threadsfield.h"

using namespace std;

int main()
{
	CThreadsField field;
	field.run();
	pthread_exit(0);
	return 0;
}
