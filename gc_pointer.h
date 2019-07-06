#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/
template <typename T, size_t size_ = 0>
class Pointer{
private:
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T>> refContainer;
    // addr points to the allocated memory to which
    // this Pointer pointer currently points.
    T *addr;
    /*  isArray is true if this Pointer points
        to an allocated array. It is false
        otherwise. 
    */
    bool isArray = false;
    // true if pointing to array
    // If this Pointer is pointing to an allocated
    // array, then arraySize contains its size.
    size_t arraySize; // size of the array
    static bool first; // true when first Pointer is created
    // Return an iterator to pointer details in refContainer.
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);
public:
    // Define an iterator type for Pointer<T>.
    typedef Iter<T> GCiterator;
    // Empty constructor
    
    Pointer(T* addr_ = nullptr);
    // Copy constructor.
    Pointer(const Pointer<T,size_> &);
    // Destructor for Pointer.
    ~Pointer();
    // Collect garbage. Returns true if at least
    // one object was freed.
    static bool collect();
    // Overload assignment of pointer to Pointer.
    T *operator=(T *t);
    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);
    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*(){
        return *addr;
    }
    // Return the address being pointed to.
    T *operator->() { return addr; }
    // Return a reference to the object at the
    // index specified by i.
    T &operator[](size_t i){ return addr[i];}
    // Conversion function to T *.
    operator T *() { return addr; }
    // Return an Iter to the start of the allocated memory.
    Iter<T> begin(){
        //int size;
        if (isArray)
            size_ = arraySize;
        else
            size_ = 1;
        return Iter<T>(addr, addr, addr + size_);
    }
    // Return an Iter to one past the end of an allocated array.
    Iter<T> end(){
        //int size;
        if (isArray)
            size_ = arraySize;
        else
            size_ = 1;
        return Iter<T>(addr + size_, addr, addr + size_);
    }
    // Return the size of refContainer for this type of Pointer.
    static size_t refContainerSize() { return refContainer.size(); }
    // A utility function that displays refContainer.
    static void showlist();
    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <typename T, size_t size_>
std::list<PtrDetails<T>> refContainerSize();
template <typename T, size_t size_>
bool Pointer<T, size_>::first = true;
template <typename T, size_t size_>
std::list<PtrDetails<T>> Pointer<T, size_>::refContainer;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,size_t size_>
Pointer<T, size_>::Pointer(T* addr_) : addr(addr_), arraySize(size_) 
{
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;

    auto it = findPtrInfo(addr_);

    // If t is already in refContainer, then
    // increment its reference count.
    // Otherwise, add it to the list.

    if(it != refContainer.end()) it->refcount++;
    else { // Create and store this entry
        PtrDetails<T> obj(addr_, size_);
        refContainer.push_front(obj);
    }

    if(size_ > 0) isArray = true;
}
// Copy constructor.
template <typename T, size_t size_>
Pointer<T, size_>::Pointer(const Pointer<T,size_> &obj){
    auto it = findPtrInfo(obj.addr);
    it->refcount++; // increment ref count
    addr = obj.addr;
    arraySize = obj.arraySize;
    if(arraySize > 0) isArray = true;
}

// Destructor for Pointer.
template <typename T, size_t size_>
Pointer<T, size_>::~Pointer(){}

// Collect garbage. Returns true if at least
// one object was freed.
template <typename T, size_t size_>
bool Pointer<T, size_>::collect(){
    bool memfreed = false;
    typename std::list<PtrDetails<T>>::iterator it;
    do {
        // Scan refContainer looking for unreferenced pointers.
        for(auto it = refContainer.begin(); it != refContainer.end(); ++it) {
        // If in-use, skip.
            if(it->refcount > 0) continue;
            memfreed = true;
        // Remove unused entry from refContainer.
            refContainer.remove(*it);
        // Free memory unless the Pointer is null.
            if(it->memPtr) {
                if(it->isArray) {
                    delete[] it->memPtr; // delete array
                }
                else {
                    delete it->memPtr; // delete single element
                }
            }
        // Restart the search.
            break;
        }
    } while(it != refContainer.end());
    
    return memfreed;
}


// Overload assignment of pointer to Pointer.
template <typename T, size_t size_>
T *Pointer<T, size_>::operator=(T *t){

    auto it {findPtrInfo(addr)};
    // First, decrement the reference count
    // for the memory currently being pointed to.
    it->refcount--;
    // Next, if the new address is already
    // existent in the system, increment its
    // count. Otherwise, create a new entry
    // for gclist.
    it = findPtrInfo(t);
    if(it != refContainer.end())
        it->refcount++;
    else {
// Create and store this entry.
        PtrDetails<T> obj(t, size_);
        refContainer.push_front(obj);
    }
    addr = t; // store the address.
    return t;
}

// Overload assignment of Pointer to Pointer.
template <typename T, size_t size_>
Pointer<T, size_> &Pointer<T, size_>::operator=(Pointer &rv){

    std::list<PtrDetails<T>> it {findPtrInfo(addr)};
    // First, decrement the reference count
    // for the memory currently being pointed to.
    it->refcount--;
    // Next, increment the reference count of
    // the new address.
    it = findPtrInfo(rv.addr);
    it->refcount++; // increment ref count
    addr = rv.addr;// store the address.
    return rv;
}

// A utility function that displays refContainer.
template <typename T, size_t size_>
void Pointer<T, size_>::showlist(){
    typename std::list<PtrDetails<T> >::iterator it;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size_ << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (auto it = refContainer.begin(); it != refContainer.end(); ++it)
    {
        std::cout << "[" << (void *)it->memPtr << "]"
             << " " << it->refcount << " ";
        if (it->memPtr)
            std::cout << " " << *it->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
// Find a pointer in refContainer.
template <typename T, size_t size_>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size_>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator it;
    // Find ptr in refContainer.
    for (it = refContainer.begin(); it != refContainer.end(); ++it)
        if (it->memPtr == ptr)
            return it;
    return it;
}
// Clear refContainer when program exits.
template <typename T, size_t size_>
void Pointer<T, size_>::shutdown(){
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator it;
    for (it = refContainer.begin(); it != refContainer.end(); ++it)
    {
        // Set all reference counts to zero
        it->refcount = 0;
    }
    collect();
}