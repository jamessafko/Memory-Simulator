/*
 Memory Simulator by James Safko
    ****Must run with command line argument: "best" or "worst"****
 This program is a memory simulator. It has a simple interface that 
 allows you to add programs to a 32 "page" memory (where
 each page represents 4KB of data). You can add data to the program, 
 "kill" running programs, view fragmentation data, and visualize the 
 memory space. Depending on the command line argument, the memory will 
 be filled either using best or worst fit management. The memory pages 
 are created through the use of linked lists. See the comments for more technical detail.
 */
#include "pa2.h"
#include <iostream>
#include <cstring>
#include <math.h>
#include <vector>
using namespace std;

int main(int argc, const char * argv[]) {
    int size, pagesNeed, userChoice = 0;
    string name, mode;
    LinkedLists memory;
    bool correctInput, finished = false;
    
    /*Checking command line arguments*/
    if(argc == 2){//and (argv1 is worst or best)
        for(int i = 1; i < argc; i++)//Assign command line argument to "mode"
            mode += argv[i];
        if(mode.compare("best") != 0 && mode.compare("worst") != 0){
            cout<<"Please enter either 'worst' or 'best' as a command line argument."<<endl;
            return 0;
        }
        cout<<"Using "<<mode<<" fit algorithm.\n"<<endl;
    }
    else{//no command line argument was passed in
        cout<<"Please enter either 'worst' or 'best' as a command line argument."<<endl;
        return 0;
    }
    
    /*Creates the free list of memory*/
    memory.createMemory();

    /*Run Program*/
    do{
        correctInput = false;
        while(correctInput == false){
            cout<<"1. Add program\n2. Kill program\n3. Fragmentation\n4. Print memory\n5. Exit\n\n";
            cout<<"choice - ";
            cin>>userChoice;//validation
            if(cin.fail()) {//If the data entered is a string, char, or non-int number
                cin.clear();//clear the instream
                cin.ignore(1000, '\n');//clear the buffer
                cout<<"\nPlease enter an integer.\n"<<endl;
            }
            else
                correctInput = true;
        }
        
        switch (userChoice) {
            case 1://add program
                name = "";
                size = 0;
                
                /*name*/
                cout<<"Program name - ";
                cin>>name;
                
                /*size*/
                correctInput = false;
                while(correctInput == false){
                    cout<<"Program size (KB) - ";
                    cin>>size;//input valid
                    if(cin.fail()) {//If the data entered is a string, char, or non-int number
                        cin.clear();//clear the instream
                        cin.ignore(1000, '\n');//clear the buffer
                        cout<<"\nPlease enter an integer for the program size.\n"<<endl;
                    }
                    else if(size == 0){
                        cout<<"\nPlease enter a program that does not take up zero memory space.\n"<<endl;
                    }
                    else
                        correctInput = true;
                }
                
                /*Checks to see if name and space are available*/
                if(memory.isRunning(name)){//Is this program already running?
                    cout<<"\nError, Program "<<name<<" already running.\n"<<endl;
                    break;
                }
                if(memory.getMemoryFree() - size < 0){//Enough memory/space?
                    cout<<"\nError, Not enough memory for Program "<<name<<"\n"<<endl;
                    break;
                }

                pagesNeed = ceil(size/4.0);//calculates the number of pages needed
                memory.updateFracture();//updates the fragment data
                
                //best fit
                if(mode.compare("best")==0){
                    if(memory.best(name, size, pagesNeed))//If there is an appropriate slot
                        cout<<"\nProgram " << name << " added successfully, " << pagesNeed << " page(s) used."<<endl<<endl;
                    else //If there is no appropriate slot
                        cout<<"There is no block of memory free large enough for "<<name<<"."<<endl;
                }
                //worst fit
                else{
                   if(memory.worst(name, size, pagesNeed))//If there is an appropriate slot
                        cout<<"\nProgram " << name << " added successfully: " << pagesNeed << " page(s) used."<<endl<<endl;
                   else //If there is no appropriate slot
                        cout<<"There is no block of memory free large enough for "<<name<<"."<<endl;
                }
                break;
            
            case 2://kill program
                cout<<"Program name - ";
                cin>>name;//input valid
                memory.deleteProg(name);
                break;
            
            case 3://fragmentation
                memory.updateFracture();
                cout << "\nThere are " << memory.getFrag() << " fragment(s).\n"<<endl;
                break;
            
            case 4://print memory
                //memory.printList();//print free
                printChart(memory);//make sure formatted correctly
                break;
            
            case 5:
                finished = true;
                break;
            
            case 6:
                memory.updateFracture();
            
            default:
                cout<<"\nPlease enter a choice between 1 and 5.\n"<<endl;
                break;
        }
    }while(finished != true);
    
    return 0;
}
