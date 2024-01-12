#include "college.h"
#include <iostream>

int main() {
    College kol;
    assert(kol.add_person<Student>("Jan", "1"));
    assert(kol.add_person<Teacher>("Jan", "2"));
    assert(kol.add_person<PhDStudent>("Jan", "3"));
    assert(kol.add_person<Student>("Jan", "4"));
    assert(!kol.add_person<Student>("Jan", "2"));
    auto jan1 = kol.find<Student>("Jan", "1").begin();
    //std::cout << jan1->get_name() << jan1->get_surname() << std::endl;
    std::cout << jan1->use_count() << std::endl;
}