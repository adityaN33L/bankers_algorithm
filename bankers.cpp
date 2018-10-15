#include <bits/stdc++.h>
using namespace std;

int main(){

	int nProc,res,queries;
	cin>>nProc>>res>>queries;
	
	int maxTime=-1;						//tracks arrival time of the last process
	int completedProcs=0;				//tracks completed processes
	int completedReq=0;					//tracks completed queries
	int remProcs=nProc;					//tracks remaining processes
	int remReq=queries;					//tracks remaining queries
	int notProcessed=0;					//tracks unprocessed requests
	map<int,int> processArrival;
	map<int,int> finished;
	map<int,vector<int> > current;
	vector<int>safeSequence(nProc);		//safe sequence in the case of safe state 
	
	//PID's inputs
	vector <int>PID(nProc);

	for(int i=0;i<nProc;i++){
		cin>>PID[i];
		finished[PID[i]]=0;
	}

	//arrival time inputs
	map<int,int> arrivalTime;
	
	for(int i=0;i<nProc;i++){
		int x;
		cin>>x;
		processArrival[PID[i]]=x;
		arrivalTime[x]=PID[i];
		maxTime=max(maxTime,x);
	}

	//available matrix input
	vector <int> available(res);

	for(int i=0;i<res;i++)
		cin>>available[i];

	//maximum matrix input
	map <int,vector<int> > maxAlloc;

	for(int i=0;i<nProc;i++){
		for(int j=0; j<res;j++ ){
			int x;
			cin>>x;
			maxAlloc[PID[i]].push_back(x);
		}
	}

	//allocation matrix input
	map <int,vector<int> > allocation;

	for(int i=0;i<nProc;i++){
		for(int j=0; j<res;j++ ){
			int x;
			cin>>x;
			allocation[PID[i]].push_back(x);
		}
	}


	//queryTable matrix input
	map <int,vector<int> > queryTable;

	for(int i=0;i<queries;i++){
		int x;
		cin>>x;
		for(int j=0; j<res;j++ ){
			int y;
			cin>>y;
			queryTable[x].push_back(y);
		}
	}

	int time=0,flagNew,flagOne,flagTwo,flagReq,flag;
	int count=0,notProc=0,curProc=0;
	map<int,int>::iterator iter;
	auto iter1=current.begin();
	int k=0;
	while(1){

		//flag to check if a new higher priority process has arrived or a request was recently granted
		flagNew=0;
		flagOne=0;
		flagTwo=0;

		//adding requests into the map only if their respective process has arrived
		for(iter=processArrival.begin(); iter != processArrival.end() ; iter++){

			if(finished[iter->first]==false && iter->second<=time){

				finished[iter->first]=1;
				if(count==0){
					flagNew=1;
				}
				//if it is not the first entry in the map
				else{
					if(iter->first<=iter1->first)
						flagNew=1;
				}

				//push the entire queryTable in map
				unsigned int len= queryTable[iter->first].size();
				for(unsigned int i=0;i<len;i++){

					current[iter->first].push_back(queryTable[iter->first][i]);
				}
				count++;
				curProc++;
			}
		}

		//if a request is granted or a new process with a high priority comes in
		//then go to the beginnning of the current map 
		if(flagReq==1 || flagNew==1)
			iter1=current.begin();

		//getting resource request of the process with highest priority
		vector <int> resReq(res);
		for(int i=0;i<res;i++)
			resReq[i]=iter1->second[i];
		

		//checking if the requested resources are less than or equal to current available resources 
		for(int i=0;i<res;i++){
			if(available[i]<resReq[i]){
				flagOne=1;
				break;
			}
		}
		
		if(flagOne==1){

			cout<<"Deny"<<endl;
			notProc++;
			flagReq=0;			//request not granted
			if(notProc==curProc){

				//Denied as many times as the process in the current maps
				//find the next process who will come in the map
				auto itnew=arrivalTime.lower_bound(time+1);
				time=itnew->first;
			}

			//reaching end of priority map start looking at the first one once again.
			if(iter1==current.end())
				flagNew=1;

			if(time>=maxTime)	
				notProcessed++;
			
			iter1++;			//request not granted hence look for the next process in the map
		}

		//request is safe and hence granted
		else{

			for(int i=0;i<res;i++){
				available[i]-=resReq[i];				//decrease available
				allocation[iter1->first][i]+=resReq[i];	//increase the resource allocation for that process
			}
			completedReq++;								//incerement the count for requests
			remReq--;

			//check if the process's maxAlloc == allocation
			for(int i=0;i<res;i++){
				if(maxAlloc[iter1->first][i]!=allocation[iter1->first][i]){
					flagTwo=1;//since it is not, the process hasn't finished
					break;
				}
			}
			iter1->second.erase(iter1->second.begin(),iter1->second.begin()+res);//remove the request from the current map
			//the process has been allocated all the resources that it needed and hence can leave its re allocation resources.
			if(flagTwo==0){

				for(int i=0;i<res;i++)
					available[i]+=maxAlloc[iter1->first][i];//increase the available resources

		
				completedProcs++;						//incrementing count for completed process
				finished[iter1->first]=1;				//mark the process as finished
				safeSequence[k++]=iter1->first;			//push it into the finished order queue
				remProcs--;
				if(iter1->second.empty()){

					current.erase(iter1);
					curProc--;
				}
			}

			//find the next process who will come in the map
			map<int,int>::iterator itnew=arrivalTime.lower_bound(time+1);
			flagReq=1;//a request was granted;
			time=itnew->first;
			notProc=0;
			notProcessed=0;
			cout<<"Grant"<<endl;			
		}

		//deadlock detection 
		//if the all the processes have completed
		if(completedProcs==nProc){
			flag=0;
			break;
		}

		//if processes not processed till last grant = number of remaining processes 
		if(notProc==remProcs){
			flag=1;
			break;
		}
	}

	if(flag==0){
		for(int i=0;i<nProc;i++)
			cout<<safeSequence[i]<<" ";
	}
	else
		cout<<"Not Safe\n";	
}