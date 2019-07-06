// This class defines an element that is stored
// in the garbage collection information list.
//
template <typename T>
class PtrDetails
{
  public:
    size_t refcount; // current reference count
    T *memPtr;         // pointer to allocated memory
    /* isArray is true if memPtr points
to an allocated array. It is false
otherwise. */
    bool isArray; // true if pointing to array
    /* If memPtr is pointing to an allocated
array, then arraySize contains its size */
    size_t arraySize; // size of array
    // Here, mPtr points to the allocated memory.
    // If this is an array, then size specifies
    // the size of the array.

    PtrDetails(T*, size_t size_ = 0);   // constructor
    bool operator==(const PtrDetails<T>& obj) const; 
};

template <typename T> 
PtrDetails<T>::PtrDetails(T *memPtr_, size_t size_)
    : memPtr(memPtr_), arraySize(size_)
    {
        refcount = 1;
        if(size_!= 0)
            isArray = true;
        else
            isArray = false;
    }

// Overloading operator== allows two class objects to be compared.
// This is needed by the STL list class.
template <typename T>
bool PtrDetails<T>::operator==(const PtrDetails<T>& obj) const
{
    return memPtr == obj.memPtr;
}