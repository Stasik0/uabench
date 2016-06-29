#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <thread>
#include <cmath>

#include "Client.h"
#include "InputParser.h"

using namespace std;

struct wokerResult
{
   unsigned int bytesTransferred = 0;
   double time = 0.0;
   unsigned int errors = 0;
   unsigned int numberOfReads = 0;
   bool finished = false;
};

void workerThread(unsigned int number, std::string& server, unsigned int count, wokerResult &result){
    unique_ptr<Client> client(new Client(server));

    //reading server time node
    client->setNodeToRead(0, 2258);
    for(unsigned int i=0;i<count;i++){
        std::pair<UA_StatusCode, double> s = client->readOnce();
        if(s.first != UA_STATUSCODE_GOOD){
            cout << "worker " << number << " failed, exiting" << endl;
            result.finished = true;
            break;
        }
        result.numberOfReads++;
        result.time += s.second;
    }

    //cout << "worker " << i << " done" << endl;
    result.finished = true;
}

void help(){
    cout << "Usage: uabench [options] [opc.tcp://]hostname:port[/endpoint]" << endl;
    cout << "Options are:" << endl;
    cout << "\t -n requests\tOverall number of requests to perform" << endl;
    cout << "\t -t threads\tNumber of threads" << endl;
}

int main(int argc, char *argv[]) {
    //parameter parsing business
    InputParser input(argc, argv);
    if(input.getOrderedParameters().size() != 1){
        cout << "uabench: wrong number of arguments" << endl;
        help();
        return 0;
    }
    if(input.getOrderedParameters().size() != 1 || input.cmdOptionExists("-h")){
        help();
        return 0;
    }

    //default parameter initialization
    unsigned int numberOfThreads = (input.getCmdOption("-t")=="" ? 2 : std::stoi(input.getCmdOption("-t")));
    unsigned int count = (input.getCmdOption("-n")=="" ? 500000 : std::stoi(input.getCmdOption("-n")));
    std::string server = input.getOrderedParameters().at(0);
    std::string prefix = "opc.tcp://";
    if(server.substr(0, prefix.size()) != prefix){
        server = prefix + server;
    }

    std::vector<wokerResult> results(numberOfThreads);
    std::vector<std::thread> threads(numberOfThreads);
    unsigned int countPerThread = count/numberOfThreads;

    cout << "Querying " << server << " for " <<  count << " requests with " << numberOfThreads << " threads" << endl;

	for(std::vector<int>::size_type i = 0; i != threads.size(); i++) {
	    cout << "Starting worker " << i << " for " << countPerThread << " requests" << endl;
	    threads.at(i) = std::thread(workerThread, i, std::ref(server), countPerThread, std::ref(results.at(i)));
	}

	//worker started, loop to query for their status
	bool all_finished = false;
	while(!all_finished){
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        all_finished = true;
        unsigned int reads = 0;
	    for(std::vector<int>::size_type i = 0; i != threads.size(); i++) {
	        reads += results.at(i).numberOfReads;
	        all_finished &= results.at(i).finished;
	    }
        float progress = (reads*1.0f / (count)) * 100;
        cout << std::ceil(progress) << "%..";
        std::cout << std::flush;
	}
	cout << endl;

	//worker finished, waiting to join to make sure threads are stopped
    for(std::vector<int>::size_type i = 0; i != threads.size(); i++) {
        threads.at(i).join();
    }

    //did all worker read required number of times?
    bool failed = false;
    for(std::vector<int>::size_type i = 0; i != threads.size(); i++) {
        if(results.at(i).numberOfReads != countPerThread){
            cout << "Worker " << i << " did not complete all reads successfully" << endl;
            failed = true;
        }
    }
    if(failed){
        return 1;
    }

    //calc stats
    double result = 0.0;
    for(std::vector<int>::size_type i = 0; i != results.size(); i++) {
        cout << "Worker " << i << " needed " << results.at(i).time/1000 << "s for " << countPerThread << " requests" << endl;
        result += (countPerThread/(results.at(i).time/1000));
    }
    //result /= numberOfThreads; //averaging

    cout << "Average performance " << result << " req/s" << endl;

	return 0;

}
