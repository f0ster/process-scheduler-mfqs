#ifndef MFQSPROCESS_H
#define MFQSPROCESS_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class mfqsProcess {
	protected:
		int pid;
	
	public:
		int burst;
		int arrival;
		int timeRemaining;
		int doneWaiting;
		int finishTime;
		int priority;
		int agefactor;
		int age;

		mfqsProcess() {
			this->pid = 0;
			this->burst = 0;
			this->arrival = 0;
			this->timeRemaining =0;
			this->doneWaiting = 0;
			this->finishTime = 0;
			this->priority = 0;
			this->agefactor = 5;
			this->age = 0;
		};
		mfqsProcess(int pid, int burst, int arrival, int priority, int agefactor) {
			this->pid = pid;
			this->burst = burst;
			this->arrival = arrival;
			this->timeRemaining = burst;
			this->priority = priority;
			this->doneWaiting = 0;
			this->finishTime = 0;
			this->agefactor = agefactor;
			this->age = 0;
		};
		~mfqsProcess() {
			
		};
		mfqsProcess( const mfqsProcess &p) {
			//cout << "p being copied from" << endl << p << endl; 
			pid = p.getPid();
			burst = p.burst;
			arrival = p.arrival;
			timeRemaining = p.timeRemaining;
			priority = p.priority;
			doneWaiting = p.doneWaiting;
			age = p.age;
			agefactor = p.agefactor;
			finishTime = p.finishTime;
		};
		mfqsProcess& operator= (const mfqsProcess &p){
			pid = p.getPid();
			age = p.age;
			agefactor = p.agefactor;
			burst = p.burst;
			arrival = p.arrival;
			timeRemaining = p.timeRemaining;
			priority = p.priority;
			doneWaiting = p.doneWaiting;
			finishTime = p.finishTime;
			return *this;
		};
		bool operator== (const mfqsProcess &p) {
			return (this->pid==p.getPid() && this->arrival == p.arrival && this->burst == p.burst);
		};
		bool operator!= (const mfqsProcess &p){
			return !(this->pid==p.getPid() && this->arrival == p.arrival && this->burst == p.burst);
		};
		friend ostream& operator<< (ostream &os, const mfqsProcess &p) {
			p.display(os);
			return os;
		};
		void display(ostream &os) const {
			os << "\t" << getPid();
			os << "\t" << burst;
			os << "\t" << arrival;
			os << "\t" << age;
			os << "\t\t" << timeRemaining;
		};
		void AGE(){
			this->age++;
		};
		bool aged(){
			return (age >= agefactor);
		};
		int getPid() const{
			return this->pid;
		};

};
#endif

