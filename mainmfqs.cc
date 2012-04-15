#include "sch.h"
#include <cstdlib>


int main(int argc, char **argv){
	if( argc == 5) {
		if( (atoi(argv[3]) < 3) || (atoi(argv[3]) > 5) )   {
			cout << "Usage: mqfs <input_file> <initial_time_quantum> <number of queues (3-5)> <agefactor>";
			return 0;
		}
	}
	if( argc != 2 && argc != 5) {
		cout << "Usage: mqfs <input_file> <initial_time_quantum> <number of queues (3-5)> <agefactor>" << endl;
		cout << "OR\nUsage: mfqs <input_file>" << endl;
		return 0;
	}
	int time = 0;
	int agefactor = 0;
	int number = 0;
		if(argc == 2){
		time = 2;agefactor = 5;number =3;
tryagain:
		cout << "Please choose a Time Quantum" << endl;
		cin >> time;
		if ( time <= 0 )
			goto tryagain;
doover:
		cout << "Please choose a number of queues(3-5)" << endl;
		cin >> number;
		if (number > 5 || number < 3)
			goto doover;
		cout << "Please choose a age factor" << endl;
		cin >> agefactor;
		}
		else {
			time = atoi(argv[2]);
			agefactor = atoi(argv[4]);
			number = atoi(argv[3]);
		}
		sch sched(time,number,agefactor);
		sched.loadinput(argv[1]);
		sched.UPDATE();
		sched.run();
		sched.stats();
		exit(0);
	return 0;
}
