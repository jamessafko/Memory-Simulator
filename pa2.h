/*
 Contains the Class "LinkedLists" used for movement of data within the memory pages.
 */

#include <iostream>
#include <vector>

#ifndef list_h
#define list_h
using namespace std;

class LinkedLists{
protected:
    struct node{
        int page, size;
        std::string name;
        node* next;
        node* prev;
    };
    
    //Heads and Tails
    node* freeHead;
    node* freeTail;
    node* usedTail;
    node* usedHead;
   
    //Current and temporary
    node* temp;
    node* freeCurr;
    node* usedCurr;
    
    //Memory information, fragmentation
    int memoryFree, pagesFree;
    int fragMinusOne;//number of memory fragments
    std::vector <int> freeChunksPageRange;//the number of contiguous pages of each fragmented block of free memory
    std::vector <node*> freeChunksNodes;//a vector of pointers the start of each free block of memory
public:
    LinkedLists();
    void createMemory();//Creates 32 doubly linked nodes to represent the free list
    //void addProg(int page, std::string name, int size);//moves a block of memory from free list to used list
    void addProg(std::string name, int size, int page, int pagesNeed);
    void deleteProg(std::string name);//clears name and size of a program and moves it's nodes to free list
    void printList();
    void printPage(int value);
    void moveMovePtr(node* &movePtr, node* &head, node* &tail, node* &curr, node* &temp, std::string name, int size);//Called by addProg and deleteProg to move nodes from one list to another
    void patchList(node* &movePtr, node* &head, node* &tail, node* &temp, node* &curr);//called by addProg and deleteProg to patch a list around a node that is about to be moved
    int getMemoryFree();
    int getFrag();
    void updateFracture();
    bool best(std::string name, int size, int pagesNeed);
    bool worst(std::string name, int size, int pagesNeed);
    bool isRunning(std::string name);
};

LinkedLists::LinkedLists(){
    freeHead = NULL;
    usedHead = NULL;
    freeTail = NULL;
    usedTail = NULL;
    memoryFree = memoryFree = 32 * 4;
    pagesFree = 32;
}

/*Creates 32 nodes in the Free list*/
void LinkedLists::createMemory(){
    for(int i = 0; i < 32; i++){
        node * n = new node;
        n->next = NULL;
        n->prev = NULL;
        n->page = i;
        n->name = "Free";
        n->size = 0;
        
        if(freeHead == NULL){
            freeHead = n;
            freeTail = n;
        }
        else{
            node* freeCurr = freeHead;
            while(freeCurr->next != NULL){
                freeCurr = freeCurr->next;
            }
            freeCurr->next = n;
            n->prev = freeCurr;
            freeTail = n;//adjusts tail to newest added
        }
    }
}

/*This method analyzes the lists and finds how many fractures there. Additionally, it updates freeChunksNodes,
 a vector where every element is the starting node of a free block of memory.
 This method also updates freeChunksPageRange, a vector where every element is the number of contiguous pages of
 memory within each free block of memory. The indexes of both these vectors correspond to the smae blocks
 of memory*/
void LinkedLists::updateFracture(){
    int contiguousPages = 0;//the number of continguous pages of memory
    freeCurr = freeHead;
    fragMinusOne = 0;
    
    //Reset vectors
    freeChunksNodes.clear();
    freeChunksPageRange.clear();
    freeChunksNodes.push_back(freeCurr);
    freeChunksPageRange.push_back(0);
    
    if (freeHead == NULL) {//If there is no more memory
        return;
    }
    int i = freeHead->page;
    do {
        if(i == freeCurr->page)//If your page is contiguous
            freeChunksPageRange[fragMinusOne] = ++contiguousPages;
        else{//If your page is a new block of free memory
            freeChunksNodes.push_back(freeCurr);
            freeChunksPageRange.push_back(0);
            fragMinusOne++;
            contiguousPages = 0;
            i = freeCurr->page;
        }
        i++;
        freeCurr = freeCurr->next;
    }while(freeCurr != NULL);
}

/*Taking in the head, tail, current, temp, and movePtr, this function will patch up the list
 before the movePtr is moved.*/
void LinkedLists::patchList(node* &movePtr, node* &head, node* &tail, node* &temp, node* &curr){
    if(movePtr == head && movePtr == tail){//if the node is the last node in used
        head = NULL;
        tail = NULL;
        temp = NULL;
    }
    else if(movePtr == head){//if moved node is head of used
        head = head->next;//move head up one
        head->prev = NULL;
        temp = NULL;
    }
    else if(movePtr == tail){//if moved node is tail of used
        tail = tail->prev;//move head up one
        tail->next = NULL;
        temp = NULL;
    }
    else{//If the node is anywhere in the list
        curr = curr->next;//move curr to node after
        temp->next = curr;//connect temp to that node to close the free used
        curr->prev = temp;
    }
}

/*Takes a node from one list passed in and places it in the other list while keeping each
 list ordered numerically by page number.*/
void LinkedLists::moveMovePtr(node* &movePtr, node* &head, node* &tail, node* &curr, node* &temp, string name, int size){
    if(head == NULL){//no program in list
        //set head
        head = movePtr;
        head->prev = NULL;
        head->next = NULL;
        
        //set tail
        tail = movePtr;
        tail->prev = NULL;
        tail->next = NULL;
        
        //sets pointers
        movePtr->next = NULL;
        movePtr->prev = NULL;
        movePtr->size = size;
        movePtr->name = name;
    }
    else{//putting in rigt location
        curr = head;
        temp = head;
        
        while(curr->next != NULL && movePtr->page > curr->page){//Loops until A. Finds the end of the list OR B. the previous page was larger
            temp = curr;
            curr = curr->next;
        }
        if(curr == head && movePtr->page < curr->page){//inserts at beginning of list
            movePtr->next = NULL;
            movePtr->prev = NULL;
            head = movePtr;
            
            curr->prev = head;
            
            head->prev = NULL;
            head->next = curr;
            
            head->size = size;
            head->name = name;
            
        }
        
        else if(curr->next == NULL && movePtr->page > curr->page){//inserts at end of list
            curr->next = movePtr;
            
            movePtr->next = NULL;
            movePtr->prev = curr;
            
            movePtr->size = size;
            movePtr->name = name;
            tail = movePtr;
        }
        
        else{//inserts anywhere between the head or tail
            //attach temp and movePtr
            temp->next = movePtr;
            movePtr->prev = temp;
            
            //attach curr and movePtr
            movePtr->next = curr;
            curr->prev = movePtr;
            
            //rename movePtr name and size
            movePtr->size = size;
            movePtr->name = name;
        }
    }
}

/*Taking in the name, size, start page, and pages needed, this function will
 call the patchList and moveMovePtr functions to move an appropriate number of nodes
 for a program from the free list to the used list.*/
void LinkedLists::addProg(string name, int size, int page, int pagesNeed){
    for(int i = 0; i < pagesNeed; i++, page++){//Repeats the process for each page needed
        freeCurr = freeHead;
        while(freeCurr->next != NULL && freeCurr->page != page){//positions the temp to be before the movePtr and the freeCurr to be on the movePtr
            temp = freeCurr;
            freeCurr = freeCurr->next;
        }
        if(freeCurr->page == page){//If the right page is selected
            memoryFree-=4;//update free memory amount
            pagesFree-=1;//update number of pages free
            node* movePtr = freeCurr;
            patchList(movePtr, freeHead, freeTail, temp, freeCurr);
            moveMovePtr(movePtr, usedHead, usedTail, usedCurr, temp, name, size);
        }
    }
}

/*Taking in the name of a program to delete, this function wipes the appropriate nodes clean in the
 used list, and using the patchList and moveMovePtr functions, moves those nodes from the used list
 and to the free list.*/
void LinkedLists::deleteProg(string name){
    usedCurr = usedHead;
    freeCurr = freeHead;
    temp = usedHead;
    int pagesReclaimed = 0;
    bool stop = false;
    
    if(usedHead == NULL && usedTail == NULL){//If the program list is empty
        cout<<"\nThere are no running programs.\n"<<endl;
        return;
    }
    
    while(!stop){
        usedCurr = usedHead;
        temp = usedHead;
        
        while(usedCurr != usedTail && usedCurr->name != name){//Loops until end of list or find right name
            temp = usedCurr;
            usedCurr = usedCurr->next;
        }
        if(usedCurr == usedTail && pagesReclaimed == 0 && usedCurr->name != name){//If the program does not exist
            stop = true;
            cout<<endl<<name<<" does not exist.\n"<<endl;
            return;
        }
        if(usedCurr == usedTail){//If at the end of the list
            stop = true;
        }
        if(usedCurr->name == name){//If you find a page with the right name
            pagesReclaimed++;//Update number of pages reclaimed
            memoryFree+=4;//Update memory size
            pagesFree+=4;//Update number of free pages
            
            node* movePtr = usedCurr;//pointer to move
            
            patchList(movePtr, usedHead, usedTail, temp, usedCurr);//Patch used list
            moveMovePtr(movePtr, freeHead, freeTail, freeCurr, temp, "Free", 0);//Move program to free list and rename name to "free" and size to 0
        }
    }
    cout<<"\nProgram "<< name <<" successfully killed: "<< pagesReclaimed <<" page(s) reclaimed.\n"<<endl;
}

/*Checks to see in a program of name "name" is alrady running. Returns true if running.*/
bool LinkedLists::isRunning(string name){
    //check to see if program is already running
    usedCurr = usedHead;
    while(usedCurr != NULL){
        if((usedCurr->name).compare(name) == 0){
            return true;
        }
        usedCurr = usedCurr->next;
    }
    return false;
}

/*Implements best fit algorithm and finds the page number of the starting
 block of the smallest chunk of free memory nodes that the program can occupy.
 Passes that page number into the addProg function. */
bool LinkedLists::best(string name, int size, int pagesNeed){
    int indexSmallest = 0;
    for(int i = 0; i < freeChunksPageRange.size(); i++){
        if(pagesNeed == freeChunksPageRange[i]){//If the chunk of space of free nodes is the exact space needed by the program
            addProg(name, size, freeChunksNodes[i]->page, pagesNeed);//passes in the pointer of the right place to add
            return true;
        }
        if(pagesNeed > freeChunksPageRange[indexSmallest]){//If the chunk of free space is less than pages needed
            indexSmallest++;//advance the counter to look at the next index
        }
        if(indexSmallest > freeChunksPageRange.size()){//If there is no page size to fit
            return false;//Exit the method
        }
        if(pagesNeed < freeChunksPageRange[i] && freeChunksPageRange[i] < freeChunksPageRange[indexSmallest])//If the current index has a smaller chunk
            indexSmallest = i;
    }
    addProg(name, size, freeChunksNodes[indexSmallest]->page, pagesNeed);//Adds program to the start page of the smallest usable chunk of free memory
    return true;
}

/*Implements best fit algorithm and finds the page number of the starting
 block of the larges chunk of free memory nodes that the program can occupy.
 Passes that page number into the addProg function. */
bool LinkedLists::worst(string name, int size, int pagesNeed){
    int indexLargest = 0;
    for(int i = 0; i < freeChunksPageRange.size(); i++){
        if(pagesNeed > freeChunksPageRange[indexLargest]){//If the chunk of free space is less than pages needed
            indexLargest++;//advance the counter to look at the next index
        }
        if(indexLargest > freeChunksPageRange.size()){//If there is no page size to fit
            return false;//Exit the method
        }
        if(pagesNeed < freeChunksPageRange[i] && freeChunksPageRange[i] > freeChunksPageRange[indexLargest]){//If the current index has a larger chunk
            indexLargest = i;
        }
    }
    addProg(name, size, freeChunksNodes[indexLargest]->page, pagesNeed);//Adds program to the start page of the smallest usable chunk of free memory
    return true;
}

/*Prints a full list of details of the used list and free list (used for debuggin).*/
void LinkedLists::printList(){
    node* curr = freeHead;
    while(curr != NULL){
        cout<<curr->page<<"\t"<<curr->name<<"\t"<<curr->size;
        cout<<"\n";
        curr = curr->next;
    }
    cout<<"\n";
    
    curr = usedHead;
    while(curr != NULL){
        cout<<curr->page<<"\t"<<curr->name<<"\t"<<curr->size;
        cout<<"\n";
        curr = curr->next;
    }
    cout<<"\n";
}

/*This method passes in the page to be printed into printPage.*/
void printChart(LinkedLists memory){
    for(int i = 1; i <= 32; i++){//Loops through all 32 pages, passing in the index to be printed into printPage()
        if(i % 8 == 1)
            cout<<"\n";
        memory.printPage(i-1);
    }
    cout<<"\n\n";
}

/*Finds the node with the right page and prints it's name.*/
void LinkedLists::printPage(int page){
    node* curr = freeHead;
    for(int i = 1; curr != NULL; i++){//loops through free pages
        if(curr->page == page){//if page number matches a page in free, prints it
            cout<<curr->name<<"\t";
            return;
        }
        curr = curr->next;
    }
    curr = usedHead;
    for(int i = 1; curr != NULL; i++){//loops through used pages
        if(curr->page == page){//if page number matches a page in free, prints it
            cout<<curr->name<<"\t";
            return;
        }
        curr = curr->next;
    }
}

int LinkedLists::getMemoryFree(){
    return memoryFree;
}

int LinkedLists::getFrag(){
    return fragMinusOne + 1;
}

#endif /* list_h */
