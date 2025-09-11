#pragma once

template<typename T>
class AutoRef
{
public:
    AutoRef() : ptr(nullptr) {}
    AutoRef(T* p) : ptr(p) { if (ptr) ptr->AddRef(); }
    AutoRef(const AutoRef& other) : ptr(other.ptr) { if (ptr) ptr->AddRef(); }
    ~AutoRef() { if (ptr) ptr->Release(); }
    
    AutoRef& operator=(T* p) {
        if (ptr != p) {
            if (ptr) ptr->Release();
            ptr = p;
            if (ptr) ptr->AddRef();
        }
        return *this;
    }
    
    AutoRef& operator=(const AutoRef& other) {
        return *this = other.ptr;
    }
    
    T* operator->() const { return ptr; }
    T& operator*() const { return *ptr; }
    operator T*() const { return ptr; }
    T** operator&() { 
        if (ptr) {
            ptr->Release();
            ptr = nullptr;
        }
        return &ptr; 
    }
    
    T** GetAddressOf() {
        if (ptr) {
            ptr->Release();
            ptr = nullptr;
        }
        return &ptr;
    }
    
    T* get() const { return ptr; }
    void reset() { *this = nullptr; }
    
    void SetPtr(T* p) {
        if (ptr) ptr->Release();
        ptr = p;
        if (ptr) ptr->AddRef();
    }
    
private:
    T* ptr;
};