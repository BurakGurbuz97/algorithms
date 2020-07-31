// Since homework indicates that OOP approach is a bonus
// I thought implementing my own vector class with sorting methods would be a good idea
// this class is derived from my previous implementation of the vector class 
// see https://github.com/BurakGurbuz97/vector_for_cpp/blob/master/vector.h for the full version
#ifndef myfile_H
#define myfile_H
#include <iostream>
#include <math.h>

namespace burak
{
    //Prototype class and the friend function beforehand
    template <class T>
    class vector;
    template<class T>
    std::ostream& operator<<(std::ostream& os, const vector<T>& V);



    template<class T>
    class vector{
        int sz;
        T* elem;
        T* temp;
        int space;
        void free_memo(int); 
        void reserve(int newalloc);

        //Private helper functions for the merge_sort
        void merge_sort_helper(T* arr, int start, int end);
        void merge(T* arr, int start,int mid, int end);
    public:
        //Assuming type T overloads the comparison operators 
        //Prototype of the sort bubblesort 
        void bubble_sort();
        //Wrapper method for the end-user, user should not pass any parameters to public method
        void merge_sort() {
            temp = new T[sz];
            merge_sort_helper(elem, 0, sz - 1);
            delete temp;
        }


        //For implementing vector data structure
        //I did not add any comments for them since
        //they are outside the scope of this course 
        vector(); 
        explicit vector(int s);
        T& operator[](int index);
        vector(std::initializer_list<T>);
        int capacity(){return space;}
        int size() const{return sz;}
        void push_back(T item);
        void pop();
        ~vector(){delete[] elem;}
        friend std::ostream& operator<< <>(std::ostream& os, const vector<T>& V);

    };

    
    //Main recursive merge sort function (as presented in the lecture materials)
    template<class T> void vector<T>::merge_sort_helper(T* arr, int start, int end){
        if(start < end) {
            int mid = floor((start + end) / 2);
            merge_sort_helper(arr, start, mid);
            merge_sort_helper(arr, mid + 1, end);
            merge(arr, start, mid, end);
        }
    }

    //Merging logic (as presented in the lecture materials)
    template<class T> void vector<T>::merge(T* arr, int start, int mid, int end){
        
        int k = start;
        int i = start;
        int j = mid + 1;

        while(k <= mid && j <= end){
            if(arr[k] <= arr[j]) {
                temp[i] = arr[k];
                k++;
            } else {
                temp[i] = arr[j];
                j++;
            }
            i++;
        }

        if (k > mid) {
            for(int h = j; h <= end; h++) {
                temp[i] = arr[h];
                i++;
            }    
        } else {
            for(int h = k; h <= mid; h++){
                temp[i] = arr[h];
                i++;
            }
        }

        for(int h = start; h <= end; h++){
            arr[h] = temp[h];
        }

    }

    // Bubble sort algorithm for the vector
    template<class T> void vector<T>::bubble_sort(){
        bool sorted = false;
        while(!sorted) {
            sorted = true;
            for(int i = 0; i < this->sz - 1; i++){
                if (elem[i] > elem[i + 1]){
                    T temp = elem[i];
                    elem[i] = elem[i + 1];
                    elem[i + 1] = temp;
                    sorted = false;
                }
            }
        }
    }



//////////////// BELOW CONTAINS CODES FOR VECTOR DATA STRUCTURE ///////////////////////////
    template<class T> std::ostream& operator<<(std::ostream& os,  vector<T>& V){
        os<<"{";
        for(int i = 0; i<V.size()-1 ; i++){
            os<<V[i]<<", ";
        }
        os<<V[V.size()-1]<<"}"<<std::endl;
        return os;
    }

    template<class T> void vector<T>::push_back(T item){
        if(space==0)
            reserve(8);
        else if (sz == space)
            reserve(2*space);
        elem[sz] = item;
        ++sz;
    }


    template<class T> void vector<T>::pop(){
        if(sz <= 1){
            std::cout<<"Cannot pop head"<<std::endl;
            return;
        }
        if(sz < space/4)
            free_memo(space/2);
        *(elem+sz-1) = 0;
        sz--;
    }

    template<class T> vector<T>::vector(){
        sz = 0;
        elem = nullptr;
        space = 0;
    }

    template<class T> vector<T>::vector(int s){     
        sz = 0;
        space = s;
        elem = new T [s];
    }

    template<class T> vector<T>::vector(std::initializer_list<T> lst){ 
        sz = lst.size();
        space = sz;
        elem =new T[sz];
        int index = 0;
        for(std::initializer_list<int>::iterator it = lst.begin(); it  < lst.end(); it++){
            elem[index++] = *it;
        }
    }


    template<class T>  T& vector<T>::operator[](int index){
        return elem[index];
    }


    template<class T> void vector<T>::free_memo(int free){
        T *p = new T[space-free];
        for(int i = 0; i < sz; i++)
            p[i] = elem[i];
        delete[] elem;
        elem = p;
        space = space-free;
    }

    template<class T> void vector<T>::reserve(int newalloc){
        if(newalloc <= space)
            return;
        T * p = new T [newalloc];
        for(int i = 0; i<sz; i++)
            p[i] = elem[i];
        delete[] elem;
        elem = p;
        space = newalloc;
    }
}
#endif