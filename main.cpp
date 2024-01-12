#include "college.h"

#include <iostream>
#include <memory>

int main() {
    // Sprawdzamy dziedziczenie. To nie jest preferowany sposób używania tych klas.
    Person *p1 = new Person("Janina", "Kowalska");
    Person *p2 = new Student("Piotr", "Nowak");
    Person *p3 = new Teacher("Wirgiliusz", "Kosobrudzki");
    Person *p4 = new PhDStudent("Cecylia", "Dorotea");

    delete p1;
    delete p2;
    delete p3;
    delete p4;

    // Tworzymy obiekt reprezentujący uczelnię i zarządzamy nią.
    College college;

    assert(college.add_course("Analysis"));
    assert(college.add_course("Algebra"));
    assert(college.add_course("Geometry"));
    assert(college.add_course("C++"));
    assert(college.add_course("Python"));
    assert(college.add_course("History", false));
    assert(college.add_course("Biology"));

    auto courses = college.find_courses("C++");
    assert(courses.size() == 1);
    assert((*courses.begin())->get_name() == "C++");

    courses = college.find_courses("A*");
    assert(courses.size() == 2);
    auto it1 = courses.begin();
    assert((*it1)->get_name() == "Algebra");
    assert((*++it1)->get_name() == "Analysis");

    courses = college.find_courses("*o?y");
    assert(courses.size() == 2);
    it1 = courses.begin();
    assert((*it1)->get_name() == "Biology");
    assert((*++it1)->get_name() == "History");

    assert(college.add_person<Student>("Jan", "Kowalski"));
    assert(!college.add_person<Teacher>("Jan", "Kowalski"));
    assert(college.add_person<Student>("Pawel", "Kowalski", false));
    assert(college.add_person<Teacher>("Jacek", "Chlebus"));
    assert(college.add_person<Teacher>("Kamil", "Litwiniuk"));
    assert(college.add_person<PhDStudent>("Alicja", "Fiszer"));
    assert(college.add_person<PhDStudent>("Jakub", "Kubanski"));

    assert(!college.add_person<Student>("Jan", "Kowalski"));
    assert(!college.add_person<Student>("Jan", "Kowalski", true));
    assert(!college.add_person<Student>("Jan", "Kowalski", false));
    assert(!college.add_person<PhDStudent>("Jan", "Kowalski"));
    assert(!college.add_person<Teacher>("Jan", "Kowalski"));
    assert(!college.add_person<Teacher>("Alicja", "Fiszer"));
    assert(!college.add_person<PhDStudent>("Jacek", "Chlebus"));

    auto people = college.find<Person>("*", "*");
    assert(people.size() == 6);
    assert((*people.begin())->get_name() == "Jacek");
    assert((*people.begin())->get_surname() == "Chlebus");

    for (auto const &p : people)
        std::clog << p->get_name() << ' ' << p->get_surname() << '\n';

    people = college.find<Person>("*", "*k*");
    assert(people.size() == 4);

    for (auto const &p : people)
        std::clog << p->get_name() << ' ' << p->get_surname() << '\n';

    assert(college.find<Student>("*", "*").size() == 4);
    assert(college.find<PhDStudent>("*", "*").size() == 2);
    assert(college.find<Teacher>("*", "*").size() == 4);

    auto jan_kowalski = *college.find<Student>("Jan", "Kowalski").begin();
    auto pawel_kowalski = *college.find<Student>("Pawel", "Kowalski").begin();
    auto alicja_fiszer = *college.find<PhDStudent>("Alicja", "Fiszer").begin();
    auto jakub_kubanski = *college.find<PhDStudent>("Jakub", "Kubanski").begin();
    auto jacek_chlebus = *college.find<Teacher>("Jacek", "Chlebus").begin();
    auto cxx = *college.find_courses("C++").begin();
    auto algebra = *college.find_courses("Algebra").begin();
    auto analysis = *college.find_courses("Analysis").begin();
    auto geometry = *college.find_courses("Geometry").begin();
    auto history = *college.find_courses("History").begin();

    assert(college.assign_course(jan_kowalski, cxx));
    assert(college.assign_course(jan_kowalski, algebra));
    assert(college.assign_course(jan_kowalski, analysis));
    assert(college.assign_course<Student>(alicja_fiszer, cxx));
    assert(college.assign_course<Teacher>(jakub_kubanski, cxx));
    assert(college.assign_course(jacek_chlebus, cxx));
    assert(college.assign_course(jacek_chlebus, algebra));
    assert(college.assign_course(jacek_chlebus, analysis));

    auto dupa = college.find<Student>(cxx);
    assert(college.find<Student>(cxx).size() == 2);
    assert(college.find<Student>(algebra).size() == 1);
    assert(college.find<Student>(geometry).size() == 0);
    assert(college.find<Teacher>(cxx).size() == 2);
    assert(college.find<Teacher>(algebra).size() == 1);
    assert(college.find<Teacher>(history).size() == 0);

    assert(alicja_fiszer->Teacher::get_courses().empty());
    auto jacek_courses = jacek_chlebus->get_courses();
    assert((*jacek_courses.begin())->get_name() == "Algebra");

    for (auto const &c : jacek_courses)
        std::clog << c->get_name() << '\n';

    assert(algebra->is_active() == true);
    college.change_course_activeness(algebra, false);
    assert(algebra->is_active() == false);

    assert(jan_kowalski->is_active() == true);
    college.change_student_activeness(jan_kowalski, false);
    assert(jan_kowalski->is_active() == false);

    try {
        college.assign_course(jan_kowalski, history);
        assert(false);
    } catch (std::exception const & e) {
        std::clog << e.what() << std::endl;
    }

    try {
        college.assign_course(pawel_kowalski, cxx);
        assert(false);
    } catch (std::exception const & e) {
        std::clog << e.what() << std::endl;
    }

    try {
        college.assign_course(jacek_chlebus, history);
        assert(false);
    } catch (std::exception const & e) {
        std::clog << e.what() << std::endl;
    }

    try {
        auto jack = std::make_shared<Student>("Jack", "London");
        college.assign_course(jack, cxx);
        assert(false);
    } catch (std::exception const & e) {
        std::clog << e.what() << std::endl;
    }

    try {
        auto jan_kowalski2 = std::make_shared<Student>("Jan", "Kowalski");
        college.assign_course(jan_kowalski2, cxx);
        assert(false);
    } catch (std::exception const & e) {
        std::clog << e.what() << std::endl;
    }

    try {
        auto cxx2 = std::make_shared<Course>("C++");
        college.assign_course(jan_kowalski, cxx2);
        assert(false);
    } catch (std::exception const & e) {
        std::clog << e.what() << std::endl;
    }

    assert((*alicja_fiszer->Student::get_courses().begin())->get_name() == "C++");
    assert(cxx->is_active() == true);
    assert(college.remove_course(cxx));
    assert(!college.change_course_activeness(cxx, true));
    assert(cxx->is_active() == false);
    assert(college.find_courses("C++").empty());
    assert((*alicja_fiszer->Student::get_courses().begin())->get_name() == "C++");
    assert(jacek_chlebus->get_courses().count(cxx) == 1);
}
