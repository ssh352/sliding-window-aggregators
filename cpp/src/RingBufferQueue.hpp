#ifndef __RING_BUFFER_QUEUE_H_
#define __RING_BUFFER_QUEUE_H_

#include<cstddef>
#include<iterator>
#include<algorithm>
#include<cassert>

template <typename E>
class RingBufferQueue {
private:
    struct ring_buffer {
        ring_buffer()
            : buffer(NULL), size(0), capacity(0), front(0), back(0) {};

        ring_buffer(size_t c)
            : buffer(new E[c]), size(0), capacity(c), front(0), back(0) {};

        ~ring_buffer() {
            if (buffer != NULL)
                delete[] buffer;
        }

        E* buffer;
        int size, capacity;
        int front, back;
    };
public:
    struct iterator {
        typedef std::input_iterator_tag iterator_category;
        typedef iterator  _Self;
        typedef E* pointer;
        typedef E& reference;
        typedef E value_type;
        typedef size_t difference_type;

        iterator()
            : aap(0), rb(NULL) {}

        iterator(size_t aap_, ring_buffer* rb_)
            : rb(rb_) {
            if (aap_ >= rb->front)
                aap = aap_;
            else
                aap = aap_ + rb->capacity;
        }

        iterator(const iterator &that)
            : aap(that.aap), rb(that.rb) {}

        _Self& operator++() {
            ++aap;
            normalize();
            return *this;
        }

        _Self operator++(int) {
            _Self tmp(*this); // copy
            operator++(); // pre-increment
            return tmp;   // return old value
        }

        _Self& operator--(){
            --aap;
            normalize();
            return *this;
        }

        _Self operator--(int) {
            _Self tmp(*this); // copy
            operator--(); // pre-increment
            return tmp;   // return old value
        }

        // special case of n == 1
        _Self operator+(difference_type __n) const {
            assert(__n == 1);
            _Self it = iterator(*this);
            ++it;
            return it;
        }

        // special case of n == 1
        _Self operator-(difference_type __n) const {
            assert(__n == 1);
            _Self it = iterator(*this);
            --it;
            return it;
        }

        _Self& operator=(const _Self& other) { // copy assignment
            if (this != &other) { // self-assignment check expected
                aap = other.aap, rb = other.rb;
                normalize();
            }
            return *this;
        }

        E& operator*() const {
            return rb->buffer[aap%rb->capacity];
        }

        E* operator->() const {
            return rb->buffer + (aap%rb->capacity);
        }

        inline void normalize() {
            if (aap >= rb->front && aap > 2*rb->capacity) {
                aap -= rb->capacity;
            }
        }

        // comparison
        friend inline bool
        operator== (const iterator &x, const iterator &y){
            int xaap = x.aap % x.rb->capacity;
            int yaap = y.aap % y.rb->capacity;
            return (xaap == yaap) && (x.rb == y.rb);
        }

        friend inline bool
        operator != (const iterator &x, const iterator &y){
            int xaap = x.aap % x.rb->capacity;
            int yaap = y.aap % y.rb->capacity;
            return (xaap != yaap) || (x.rb != y.rb);
        }
        // where in the world are we?
        size_t aap; // "adjusted" absolute position
        ring_buffer* rb; // the actual ring buffer
    };

    RingBufferQueue()
        : _rb(new ring_buffer(MAGIC_MINIMUM_RING_SIZE)) {
    }

    ~RingBufferQueue() {
        delete _rb;
    }

    size_t size() { return _rb->size; }

    void push_back(E elem) {
        int n = size();
        if (n+1 >= _rb->capacity)
            rescale_to(THRES*_rb->capacity, n+1);

        _rb->size++;
        _rb->buffer[_rb->back++] = elem;

        if (_rb->back >= _rb->capacity)
            _rb->back -= _rb->capacity;
    }

    void pop_front() {
        _rb->front++;
        if (_rb->front >= _rb->capacity)
            _rb->front -= _rb->capacity;
        _rb->size--;

        int n = size();
        if (n <= _rb->capacity/(2*THRES))
            rescale_to(_rb->capacity/THRES, n);
    }

    E front() {
        assert(size() > 0);
        return _rb->buffer[_rb->front];
    }

    E back() {
        assert(size() > 0);
        int bi = _rb->back - 1;
        if (bi < 0)
            bi += _rb->capacity;
        return _rb->buffer[bi];
    }

    const iterator begin() { return iterator(_rb->front, _rb); }
    const iterator end() { return iterator(_rb->front + _rb->size, _rb); }


private:
    const int THRES = 2;
    const int MAGIC_MINIMUM_RING_SIZE = 4;

    void rescale_to(size_t new_size, size_t ensure_size) {
        new_size = std::max(new_size, (size_t) MAGIC_MINIMUM_RING_SIZE);

        if (ensure_size > new_size) { throw 1; } // this should never happen

        E* rescaled_buffer = new E[new_size];

        // copy while preserving the positions (modulo new_size)
        size_t old_cap = _rb->capacity;
        int n = size(), f_src = _rb->front, f_dst = _rb->front;
        for (int index=0;index<n;++index) {
            rescaled_buffer[f_dst++] = _rb->buffer[f_src++];
            if (f_dst >= new_size) f_dst -= new_size;
            if (f_src >= old_cap) f_src -= old_cap;
        }

        std::swap(rescaled_buffer, _rb->buffer);

        delete[] rescaled_buffer;

        _rb->front %= new_size, _rb->back = (_rb->front + n)%new_size;
        _rb->capacity = new_size;
    }
public:
    ring_buffer* _rb;
};

#endif
