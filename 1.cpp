
#include <iostream>
#include <utility>

// Класс точки на плоскости
class MyPoint {
private:
    int x, y;

public:
    MyPoint(int x, int y) : x(x), y(y) {
        std::cout << "MyPoint created: x=" << x << ", y=" << y << "\n";
    }

    ~MyPoint() {
        std::cout << "MyPoint destroyed: x=" << x << ", y=" << y << "\n";
    }

    void print() const {
        std::cout << "Point: (" << x << ", " << y << ")\n";
    }
};

// Шаблон MyUnique (аналог std::unique_ptr)
template<class T>
class MyUnique {
private:
    T* ptr;

public:
    // Конструктор с сырым указателем
    explicit MyUnique(T* p = nullptr) : ptr(p) {}

    // Деструктор
    ~MyUnique() {
        delete ptr;
    }

    // Запрет копирования
    MyUnique(const MyUnique&) = delete;
    MyUnique& operator=(const MyUnique&) = delete;

    // Конструктор перемещения
    MyUnique(MyUnique&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    // Оператор присваивания с перемещением
    MyUnique& operator=(MyUnique&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // Метод get
    T* get() const {
        return ptr;
    }

    // Оператор разыменования
    T& operator*() {
        return *ptr;
    }

    // Оператор доступа к членам
    T* operator->() {
        return ptr;
    }
};

// Шаблон MyShared (аналог std::shared_ptr)
template<class T>
class MyShared {
private:
    T* ptr;
    size_t* ref_count; // Счётчик ссылок

    void release() {
        if (ref_count) {
            --(*ref_count);
            if (*ref_count == 0) {
                delete ptr;
                delete ref_count;
            }
        }
    }

public:
    // Конструктор с сырым указателем
    explicit MyShared(T* p = nullptr) : ptr(p), ref_count(p ? new size_t(1) : nullptr) {}

    
    ~MyShared() {
        release();
    }

    
    MyShared(const MyShared& other) : ptr(other.ptr), ref_count(other.ref_count) {
        if (ref_count) {
            ++(*ref_count);
        }
    }

    
    MyShared& operator=(const MyShared& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            if (ref_count) {
                ++(*ref_count);
            }
        }
        return *this;
    }

    
    MyShared(MyShared&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    
    MyShared& operator=(MyShared&& other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }

    
    T* get() const {
        return ptr;
    }

    
    T& operator*() {
        return *ptr;
    }

    
    T* operator->() {
        return ptr;
    }

    
    size_t use_count() const {
        return ref_count ? *ref_count : 0;
    }
};


template<typename T, typename... Args>
MyUnique<T> Make_MyUnique(Args&&... args) {
    return MyUnique<T>(new T(std::forward<Args>(args)...));
}


template<typename T, typename... Args>
MyShared<T> Make_MyShared(Args&&... args) {
    return MyShared<T>(new T(std::forward<Args>(args)...));
}

int main() {
    
    std::cout << "=== MyUnique Demo ===\n";
    {
       
        auto uniquePtr = Make_MyUnique<MyPoint>(5, 10);
        uniquePtr->print();
        (*uniquePtr).print();
        std::cout << "Raw pointer: " << uniquePtr.get() << "\n";

       
        auto uniquePtr2 = std::move(uniquePtr);
        if (!uniquePtr.get()) {
            std::cout << "uniquePtr is now null after move\n";
        }
        uniquePtr2->print();
    } 

    std::cout << "\n=== MyShared Demo ===\n";
    {
     
        auto sharedPtr1 = Make_MyShared<MyPoint>(3, 4);
        sharedPtr1->print();
        std::cout << "Shared count: " << sharedPtr1.use_count() << "\n";

        
        auto sharedPtr2 = sharedPtr1;
        std::cout << "After copy, shared count: " << sharedPtr1.use_count() << "\n";
        sharedPtr2->print();

       
        auto sharedPtr3 = std::move(sharedPtr2);
        std::cout << "After move, shared count: " << sharedPtr1.use_count() << "\n";
        if (!sharedPtr2.get()) {
            std::cout << "sharedPtr2 is now null after move\n";
        }
        sharedPtr3->print();
    } 

    return 0;
}
