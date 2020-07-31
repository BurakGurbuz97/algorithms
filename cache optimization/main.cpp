#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

class Node {
public:
	int element;
	Node* next;
	Node* prev;
    Node(int elem) {
        element = elem;
        next = NULL;
        prev = NULL;
    }
};

//Wrapper Class for node class
class list {
    Node* head;
    Node* tail;
public:
    list(){head = NULL; tail = NULL;}
    void move_front(Node* n); //O(1)
    void push_back(Node* node_address); //O(n)
    Node* get_last();//O(1)
};

Node* list::get_last(){
    return tail;
}

void list::move_front(Node* n){
    if (n == head)
        return;
    else if (n == tail) {
        n->prev->next = NULL;
        tail =  n->prev;
    } else {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }
    head->prev = n;
    n->next = head;
    n->prev = NULL;
    head = n;
}

void list::push_back(Node* node_address) {
    if (head == NULL) {
        head = node_address;
        tail = node_address;
    } else {
		tail->next = node_address;
		node_address->prev = tail;
		node_address->next = NULL;
		tail = node_address;
	}
}

int main(int argc, char** argv) {
	ifstream testfile;
	testfile.open(argv[1]); //read the inputs from test file

	int capacity; //capacity is the maximum number of elements in the cache, defined as k in pdf
	int element_number; //number of different elements that can possibly requested, equals to k+1 in our case
	int request_number; //number of requests, defined as n in pdf

	testfile >> capacity;
	element_number = capacity + 1;

	int cache_size = 0; //cache size keeps number of elements in the cache, initially zero
	vector<int> cache(element_number, 0); //cache is created as empty as all indexes are 0

	testfile >> request_number;
	vector<int> requests(request_number);
	for (int i = 0; i < request_number; i++)
		testfile >> requests[i];
	
	vector<int> fif(requests.size()); //to keep the farthest element for each request

	vector<int> elements(element_number); //elements array {0, 1, ... , k}
	for (int i = 0; i < element_number; i++)
		elements[i] = i;
	
	vector<Node*> addresses; //array for keeping the address of double linked list nodes of elements
	//addresses[i] keeps the node address for elements[i]
	
	list linkedList;
		// 1. create the double linked list for elements vector, you can use Node class defined above
		// keep each node's address in addresses array
		for(int i = 0; i < elements.size(); i++){
			Node* n = new Node(elements[i]);
			addresses.push_back(n);
			linkedList.push_back(n);
		}
		
		// 2. fill the fif (farthest-in-future) array considering the pseudocode given in pdf.
		for(int i = request_number;  i >= 0; i--){
			Node* n = addresses[requests[i]];
			linkedList.move_front(n);
			fif[i] = linkedList.get_last()->element;
		}
		
		// 3. process the requests and print out necessary outputs, you can use following syntax:
		for(int i = 0;  i < request_number; i++){
			if (cache[requests[i]] == 0) {
				cout << "cache miss" << endl;
				if (cache_size == capacity) {
					cache[fif[i]] = 0;
					cache[requests[i]] = 1;
					cout << "cache is full, element " << fif[i] << " is evicted" << endl;
				} else {
					cache[requests[i]] = 1;
					cache_size++;
				}
				cout << "element " << requests[i] << " is added into the cache" << endl;
			} else {
				cout << "cache hit" << endl;
			}
		}
	return 0;
}