#include <iostream>
#include "include/ChunkMemoryResource.h"
#include "include/Vector.h"

struct ComplexType {
    int id;
    double value;
    std::string name;
    
    ComplexType(int i, double v, const std::string& n) 
        : id(i), value(v), name(n) {}
};

int main() {
    std::cout << "=== Int Vector Demo ===" << std::endl;
    Vector<int> int_vec;
    for (int i = 0; i < 5; ++i) {
        int_vec.push_back(i * 10);
    }
    
    for (auto it = int_vec.begin(); it != int_vec.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "\n=== ComplexType Vector Demo ===" << std::endl;
    Vector<ComplexType> complex_vec;
    complex_vec.push_back(ComplexType(1, 3.14, "First"));
    complex_vec.push_back(ComplexType(2, 2.71, "Second"));
    
    for (auto it = complex_vec.begin(); it != complex_vec.end(); ++it) {
        std::cout << "ID: " << it->id << ", Value: " << it->value 
                  << ", Name: " << it->name << std::endl;
    }

    std::cout << "\n=== Custom Memory Resource Demo ===" << std::endl;
    ChunkMemoryResource custom_resource;
    auto alloc = std::pmr::polymorphic_allocator<int>(&custom_resource);
    Vector<int, std::pmr::polymorphic_allocator<int>> custom_vec{alloc};
    
    for (int i = 0; i < 3; ++i) {
        custom_vec.push_back(i * 100);
    }
    
    for (auto it = custom_vec.begin(); it != custom_vec.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    return 0;
}