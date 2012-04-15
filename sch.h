#ifndef sch_H
#define sch_H

#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include "mfqsProcess.h"


using namespace std;
class sch {
	protected:
		vector<mfqsProcess> future_list;
		vector<mfqsProcess> done_list;
		vector<int> cpu_hist;
		int timeQ;
		int queue_total;
		int current_queue;
		int agefactor;
		vector<mfqsProcess> queues[5]; // is this right?	
		bool interrupt;
		int thetime;
		
	public:

		sch(int time, int number,int agefactor) 
		{ 
			cpu_hist.resize(100000);
			for (int i=0;i<queue_total;i++){
				queues[i].resize(100);
			}
			thetime = 0;
			timeQ = time;
			queue_total = number;
			interrupt = false;
			current_queue = 0;
			this->agefactor = agefactor;
		};
		~sch() { };

		class proxyCompareArrival {
			sch& that;
			public:
			proxyCompareArrival(sch &h) : that(h) {}
			bool operator()(mfqsProcess const &p1,mfqsProcess const &p2) const {
				return (p1.arrival  <= p2.arrival );
			}
		};
		int loadinput(char *filename) {
			ifstream testfile(filename); 
			//make sure file exists before calling sed
			if (!testfile) {
				cerr << "error loading input file!\n";
				return 0;
			}
			testfile.close();
			string cmd = "sed \'s/\t/ /g\' " + string(filename) + " > sample.clean";
			system(cmd.c_str());
			ifstream infile("sample.clean");
			if (!infile){
				cerr << "error loading file after clean-up!\n";
				return 0;
			}

			vector<string> input;
			string tmp_str;
			while( !infile.eof() )
			{
				getline(infile, tmp_str);
				input.push_back(tmp_str);
			}
			infile.close();
			system("rm sample.clean");

			//all lines loaded into string vector :input:
			//now parse through string vector and make mfqsProcess vector
			for(unsigned int i=0; i < input.size() ; i++) {
				int pid=0,burst=0,arrv=0,priorty=0;
				char *pch;
				/*char str[ strlen(input[i].c_str())+1 ];
				strcpy (str,input[i].c_str());*/
				char *str = new char[strlen(input[i].c_str())+1];
				strcpy (str,input[i].c_str());
				
				pch = strtok (str," ");
				int count = 1;
				while(pch!=NULL && count < 6){
					if ( atoi(pch) != 0 ) { // make sure it is a number
						if (count == 1)
							pid=atoi(pch);
						if (count == 2)
							burst=atoi(pch);
						if (count == 3)
							arrv=atoi(pch);
						if (count == 4)
							priorty=atoi(pch);
					}
					count++;
					pch = strtok(NULL," ");
				}
				delete[] str;
				if (pid != 0)
					future_list.push_back(mfqsProcess(pid,burst,arrv,priorty,agefactor));
			}
				//sort( future_list.begin(), future_list.end() , proxyCompareArrival(*this) );
			//print_all(0);
			
			return 0;
		};
		
		void UPDATE(){
			// aging
			//vector<mfqsProcess>::iterator it = queues[this->queue_total - 1].begin();
			//for(it = queues[this->queue_total - 1].begin(); it != queues[this->queue_total - 1].end(); it++){
			mfqsProcess* it = &queues[this->queue_total-1][0];
			for(unsigned int i=0; i<queues[this->queue_total-1].size(); i++){
				it = &queues[this->queue_total-1][i];
				it->AGE();
				if(it->aged())
				{
					it->age = 0;
					queues[this->queue_total - 2].push_back( mfqsProcess(*it) );
					queues[this->queue_total - 1].erase( queues[this->queue_total-1].begin() + i );
					if( current_queue == queue_total)
						interrupt = true;
				}
			}
			// future cheching
			//mfqsProcess* it = &future_list[0];
			for(unsigned int i=0; i < future_list.size() ; i++){
				it= &future_list[i];
				if(thetime >= it->arrival)
				{
					//cout << "new proc from future " << endl << *it << endl;
					queues[0].push_back(*it);
					future_list.erase( future_list.begin()+i );
					if( current_queue != 0)
						interrupt = true;
				}	
			}	
		}
		
		void run(){
			bool done = false;
			//int test=0;
			while(!done){
				/*cout << "?" << endl;
				cin >> test;
				if (test==1)
					exit(1);
				if (test==0)
					print_all(0);*/
				interrupt = false;
				for( int i=0; i < queue_total && !interrupt ; i++ ) {
					current_queue = i;
					if (!queues[i].empty() && i != queue_total-1)
						do_rr_q(i); // do rr on q(i) 
					else {
						if(!queues[i].empty() && i == queue_total-1)
							do_fcfs(i);
						else //evertying empty
							if (!future_list.empty()){
								update_clock();
								cpu_hist.push_back(-1);
							}
							else{
								bool allEmpty=true;
								for(int j=0; j<queue_total && allEmpty; j++){
									if (!queues[j].empty())
										allEmpty=false;
								}
								if (allEmpty)
									done=true;
							}
					}
				}
			}
			print_all(0);
		};
		
		
		int do_rr_q(int i){
			//cout << "do_rr_q(" << i << ")..." << endl;
			int subtime = 0;
			while(!interrupt && !queues[i].empty()){
				mfqsProcess* first = &queues[i][0];
				if (first->burst == first->timeRemaining)
					first->doneWaiting = thetime;
				for(subtime = 0; (subtime < timeQ*(i+1)) && !interrupt && first->timeRemaining > 0; subtime++){
					//cout << "pid " << first->getPid() << " remaining " << first->timeRemaining << endl; 	
					first->timeRemaining--;
					cpu_hist.push_back(first->getPid());
					update_clock(); //calls update;
					first = &queues[i][0];
				}			
				if( first->timeRemaining <= 0){
					first->finishTime = thetime;
					done_list.push_back( *first );
					queues[i].erase( queues[i].begin() );
				} else{
					//cout << "moving pid " << first->getPid() << " to [" << i << "]" << endl;
					queues[i+1].push_back( mfqsProcess(*first) ); //push
					queues[i].erase( queues[i].begin() );				//pop
				}
			}
			return (int)interrupt;
		};
		int do_fcfs(int i) {
				//cout << "do_fcfs(" << i << ")..." << endl; 		
			while(!interrupt && !queues[i].empty()){
				mfqsProcess* first = &queues[i][0];
				for(; !interrupt && first->timeRemaining > 0 ;){
					first->timeRemaining--;
					cpu_hist.push_back(first->getPid());
					update_clock();
					first = &queues[i][0];
				}
				if (first->timeRemaining <= 0 ){
					first->finishTime = thetime;
					done_list.push_back( *first );
					queues[i].erase( queues[i].begin() );
				}
			}
			return (int)interrupt;
		};
		void update_clock(){
			//cout << "clock "<< thetime+1 <<endl;
			thetime++;
			UPDATE();
		};
		void print_all(int k){
			vector<mfqsProcess>::iterator fut;
			vector<mfqsProcess>::iterator active;
			cout << "Future Processes" << endl;
			for(fut = future_list.begin(); fut != future_list.end(); fut++){
				cout << *fut << endl;
			}
			for(int i=0; i < queue_total; i++){
				cout << "Active Process Queue[" <<i<<"]" << endl;
				for(active = queues[i].begin(); active != queues[i].end(); active++){
					cout << *active << endl;
				}
			}
			cout << "CPU history" <<endl;
			for(unsigned int i=0; i<cpu_hist.size(); i++){
				cout << cpu_hist[i] << " ";
			} cout << endl;
		};
	
		void stats() {
			double avgTurn = 0;
			double avgWait = 0;
			double count = 0;
			vector<mfqsProcess>::iterator it;
			for(it = done_list.begin(); it != done_list.end(); it++){
				count++;
				avgTurn += (it->finishTime+1 - it->arrival);
				avgWait += (it->doneWaiting - it->arrival);
			}
			avgTurn /= count;
			avgWait /= count;
			cout << "Average Turnaround Time: " << avgTurn << endl;
			cout << "Average Waiting Time: " << avgWait << endl;
		
			//cout << avgTurn << "\t" << avgWait << endl;
		};
};




#endif
