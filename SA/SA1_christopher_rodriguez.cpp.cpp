//Christopher M. Rodriguez, SA 1, CS 280-002, cmr82@njit.edu


#include <iostream>
#include <sstream>
using namespace std;

int main(){
    string response;
    string firstName;
    string lastName;
    int section;
    cout<<"Welcome to CS 280 (Spring 2025)"<<endl;
    cout<<"What are your first name, last name, and section number?"<<endl;

    getline(cin, response);
    istringstream iss(response);

    iss >> firstName >> lastName >> section;
    cout<< "Hello " <<firstName << ", Welcome to CS 280 Section " << section<<endl;
   
    return 0;
}

