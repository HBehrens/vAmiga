// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* The emulator uses buffers at various places. Most of them are derived from
 * one of the following two classes:
 *
 *           RingBuffer : A standard ringbuffer data structure
 *     SortedRingBuffer : A ringbuffer that keeps the entries sorted
 */

#ifndef _BUFFERS_INC
#define _BUFFERS_INC

template <class T, int capacity> struct RingBuffer
{
    // Element storage
    T elements[capacity];

    // Read and write pointers
    int r, w;

    //
    // Constructing and initializing
    //

    RingBuffer() { clear(); }
    void clear() { r = w = 0; }

    //
    // Serializing
    //

    template <class W>
    void applyToItems(W& worker)
    {
        worker & elements & r & w;
    }

    //
    // Querying the fill status
    //

    int count() const { return (capacity + w - r) % capacity; }
    bool isEmpty() const { return r == w; }
    bool isFull() const { return count() == capacity - 1; }

    //
    // Working with indices
    //

    int begin() const { return r; }
    int end() const { return w; }
    static int next(int i) { return (capacity + i + 1) % capacity; }
    static int prev(int i) { return (capacity + i - 1) % capacity; }


    //
    // Reading and writing elements
    //

    T& current()
    {
        return elements[r];
    }

    T& read()
    {
        assert(!isEmpty());

        int oldr = r;
        r = next(r);
        return elements[oldr];
    }

    void write(T element)
    {
        assert(!isFull());

        int oldw = w;
        w = next(w);
        elements[oldw] = element;
    }


    //
    // Debugging
    //

    /*
    void dump()
     {
         printf("%d elements (r = %d w = %d):\n", count(), r, w);
         for (int i = r; i != w; i = next(i)) {
             assert(i < capacity);
             printf("%2i: ", i);
             elements[i].print();
         }
         printf("\n");
     }
     */
};

template <class T, int capacity>
struct SortedRingBuffer : public RingBuffer<T, capacity>
{
    // Key storage
    i64 keys[capacity];

    // Inserts an element at the right position
    void insert(i64 key, T element)
    {
        assert(!this->isFull());

        // Add the new element
        int oldw = this->w;
        this->write(element);
        keys[oldw] = key;

        // Keep the elements sorted
        while (oldw != this->r) {

            // Get the index of the preceeding element
            int p = this->prev(oldw);

            // Exit the loop once we've found the correct position
            if (key >= keys[p]) break;

            // Otherwise, swap elements
            swap(this->elements[oldw], this->elements[p]);
            swap(keys[oldw], keys[p]);
            oldw = p;
        }
    }

    void dump()
    {
        printf("%d elements (r = %d w = %d):\n", this->count(), this->r, this->w);
        for (int i = this->r; i != this->w; i = this->next(i)) {
            assert(i < capacity);
            printf("%2i: [%lld] ", i, this->keys[i]);
            printf("%d\n", this->elements[i]);
        }
        printf("\n");
    }
};


/* Register change recorders
 * For certain registers, Agnus and Denise have to keep track about when a
 * value changes. This information is stored in sorted ring buffers called
 * register change recorders.
 */
struct RegChange
{
    u32 addr;
    u16 value;

    template <class T>
    void applyToItems(T& worker)
    {
        worker & addr & value;
    }

    // Constructors
    RegChange() : addr(0), value(0) { }
    RegChange(u32 a, u16 v) : addr(a), value(v) { }

    void print()
    {
        printf("addr: %x value: %x\n", addr, value);
    }
};

template <int capacity>
struct RegChangeRecorder : public SortedRingBuffer<RegChange, capacity>
{
    // Returns the closest trigger cycle
    Cycle trigger() {
        return this->isEmpty() ? NEVER : this->keys[this->r];
    }

    void dump()
    {
        printf("%d elements (r = %d w = %d):\n", this->count(), this->r, this->w);
        for (int i = this->r; i != this->w; i = this->next(i)) {
            assert(i < capacity);
            printf("%2i: [%lld] ", i, this->keys[i]);
            this->elements[i].print();
        }
        printf("\n");
    }
};

#endif