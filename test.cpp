#include "college.h"
#include <iostream>

int main() {
    Course c("zupa", false);
    std::cout << c.get_name() << std::endl;
    std::cout << c.is_active() << std::endl;
    c.change_activeness(false);
    std::cout << c.is_active() << std::endl;

    Student s("Jan", "XIV");
    std::cout << s.get_name() << " " << s.get_surname() << std::endl;

    Teacher t("Bob", "VII");
    std::cout << t.get_name() << " " << t.get_surname() << std::endl;

    PhDStudent p("Tom", "MXII");
    std::cout << p.get_name() << " " << p.get_surname() << std::endl;
}