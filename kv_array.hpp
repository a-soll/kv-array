#ifndef KV_ARRAY_H
#define KV_ARRAY_H

#include <iterator>

namespace kv {

template <typename T, size_t size>
class array {
public:
    // Iterator
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T *;
        using reference         = T &;

        Iterator(pointer ptr)
            : _ptr(ptr) {}

        inline reference operator*() const {
            return *this->_ptr;
        }
        inline reference operator*() {
            return *this->_ptr;
        }
        inline pointer operator->() {
            return this->_ptr;
        }
        inline const pointer operator->() const {
            return this->_ptr;
        }

        Iterator operator++() {
            Iterator tmp = *this;
            this->_ptr++;
            return tmp;
        }
        Iterator operator++(int) {
            this->_ptr++;
            return *this;
        }

        inline friend bool operator==(const Iterator &a, const Iterator &b) {
            return a._ptr == b._ptr;
        }
        inline friend bool operator!=(const Iterator &a, const Iterator &b) {
            return a._ptr != b._ptr;
        }

    private:
        pointer _ptr;
    };
    // Iterator end
    array() {}
    ~array() {}

    inline Iterator begin() {
        return Iterator(&this->_arr[0]);
    }
    inline Iterator end() {
        return Iterator(&this->_arr[this->_ind]);
    }
    inline const Iterator begin() const {
        return Iterator(&this->_arr[0]);
    }
    inline const Iterator end() const {
        return Iterator(&this->_arr[size - 1]);
    }
    /**
     * `UNSAFE:` sets all indices to `nullptr`
     */
    inline void zero_initialize() {
        for (size_t i = 0; i < size; i++) {
            this->_arr[i] = nullptr;
        }
    }
    /**
     * pushes items back and tracks length
     */
    void inline push_back(const T &item) {
        this->_arr[this->_ind] = item;
        this->_ind++;
    }
    /**
     * `UNSAFE:` cannot use `.length()` with this.
     */
    inline void insert(const T &item, size_t at) {
        this->_arr[at]            = item;
        this->_trustworthy_length = false;
    }
    /**
     * `UNSAFE:` deallocates memory and assigns it to `nullptr`
     *
     * frees the item at given index
     */
    inline void free_at(size_t at) {
        delete this->_arr[at];
        this->_arr[at] = nullptr;
    }
    /**
     * `UNSAFE:` probably breaks `.length()`, so don't rely on it.
     *
     * index wraps to beginning to add new items after hitting capacity.
     * `RETURN:` index of the item inserted. ignorable.
     */
    size_t wrap_back(const T &item) {
        this->_arr[this->_ind] = item;
        this->_ind++;
        switch (this->_ind == size) {
        case true:
            this->_ind = 0;
            return size - 1;
        default:
            return this->_ind - 1;
        }
        this->_trustworthy_length = false;
    }
    /**
     * `UNSAFE:` probably breaks `.length()`, so don't rely on it.
     *
     * index wraps to beginning to add new items after hitting capacity.
     * deallocates currently occupied memory. need to call zero_initialize array to make
     * this work.
     * `RETURN:` index of the item inserted. ignorable.
     */
    size_t write_back(const T &item) {
        this->_trustworthy_length = false;
        if (this->_arr[this->_ind] != nullptr) {
            this->free_at(this->_ind);
        }
        this->_arr[this->_ind] = item;
        this->_ind++;
        if (this->_ind == size) {
            this->_ind = 0;
            return size - 1;
        }
        return this->_ind - 1;
    }
    /**
     * `UNSAFE:` deallocates memory and assigned it to `nullptr`
     *
     * same as `clear()`, but deallocates objects.
     */
    inline void free() {
        size_t end = this->_trustworthy_length ? this->_ind : size;
        for (size_t i = 0; i < end; i++) {
            this->free_at(i);
        }
        this->_ind = 0;
    }
    /**
     * `RETURN:` number of items currently in array
     */
    inline size_t length() const {
        return this->_ind;
    }
    /**
     * `RETURN:` maximum amount of items that array can hold.
     */
    inline size_t capacity() const {
        return size;
    }
    /**
     * resets array index to 0. does not free or remove items.
     */
    inline void clear() {
        this->_ind = 0;
    }
    inline T &back() {
        return this->_arr[this->_ind - 1];
    }
    inline const T &back() const {
        return this->_arr[this->_ind - 1];
    }

    inline T &operator[](size_t ind) {
        return this->_arr[ind];
    }
    inline const T &operator[](size_t ind) const {
        return this->_arr[ind];
    }
    array &operator=(const array &rhs) {
        if (this == &rhs) {
            return *this;
        }
        for (size_t i = 0; i < rhs.capacity(); i++) {
            this->_arr[i] = rhs[i];
        }
        this->_ind                = rhs._ind;
        this->_trustworthy_length = rhs._trustworthy_length;
        return *this;
    }

private:
    size_t _ind              = 0;
    bool _trustworthy_length = true;
    T _arr[size];
};

} // namespace kv
#endif /* KV_ARRAY_H */
