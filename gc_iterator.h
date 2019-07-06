/*Iterator class for pointer*/

#pragma once

// Exception thrown when an attemp is made
// to use and Iter that exceeds the range
// of the underlying object

class OutOfRangeExc
{
    // Add functionality if needed by your applicatoin
};

/* An iterator-like class for cycling through arrays
that are pointed to by GCPtrs. Iter pointers DO NOT participate
in or affect garbage collection. Thus, an Iter pointing to some
object does not prevent that object from being recycled
*/

template <class T>
class Iter
{
    T *ptr;            // current pointer value
    T *end;            // points to element one past end
    T *begin;          // point to start of allocated arrays
    long unsigned int length = 0; // length of sequence
public:
    Iter(T *ptr_ = nullptr, T *end_ = nullptr, T *begin_ = nullptr)
            : ptr(ptr_), end(end_), begin(begin_)
    {
        length = end_ - begin_;
    }

    // Returns length of sequence to which this Iter pointers
    long unsigned int size() { return length; }

    // Return value pointed to by ptr.
    // Do not allow out-of-bounds access
    T &operator*()
    {
        if (ptr >= end || ptr < begin)
            throw OutOfRangeExc();
        return *ptr;
    }

    // Return address contained in ptr.
    // Do not allow out-of-bounds access
    T *operator->()
    {
        if (ptr >= end || ptr < begin)
            throw OutOfRangeExc();
        return ptr;
    }

    // Prefix ++
    Iter operator++()
    {
        ptr++;
        return *this;
    }

    // Prefix --
    Iter operator--()
    {
        ptr--;
        return *this;
    }

    // postfix ++
    Iter operator++(int notused)
    {
        T *tmp  = ptr;
        ptr++;
        return Iter<T>(tmp, begin, end);
    }

    Iter operator--(int notused)
    {
        T *tmp = ptr;
        ptr--;
        return Iter<T>(tmp, begin, end);
    }

    // Return a reference to the object at the
    // specified index. Do not allow out-ofbounds
    // access
    T &operator[](long unsigned int i)
    {
        if (i < 0 || i >= length)
            throw OutOfRangeExc();
        return ptr[i];
    }

    // Define the relational operators
    bool operator==(Iter obj) const { return ptr = obj.ptr; }
    bool operator!=(Iter obj) const { return ptr != obj.ptr; }
    bool operator>(Iter obj) const { return ptr > obj.ptr; }
    bool operator>=(Iter obj) const { return ptr >= obj.ptr; }
    bool operator<(Iter obj) const { return ptr < obj.ptr; }
    bool operator<=(Iter obj) const { return ptr <= obj.ptr; }

    Iter operator-(long unsigned int n)
    {
        ptr -= n;
        return *this;
    }

    Iter operator+(long unsigned int n)
    {
        ptr += n;
        return *this;
    }

    // Return number of elements between two iterators
    long unsigned int operator-(const Iter<T> &it) { return ptr - it.ptr; }
};
