//
// Created by Kacper Jablonski on 24/12/2023.
//

#ifndef COLLEGE_COLLEGE_H
#define COLLEGE_COLLEGE_H


#include <string>
#include <utility>
#include <vector>
#include <iostream>

class Course;
class Person;

using CoursesCollection = std::vector<std::shared_ptr<Course>>;
using PeopleCollection = std::vector<std::shared_ptr<Person>>;

class Person {
    private:
    std::string name;
    std::string surname;

    public:
    Person(std::string name, std::string surname) {
        this->name = std::move(name);
        this->surname = std::move(surname);
    }

    virtual ~Person() = default;

    std::string get_name() {
        return name;
    }

    std::string get_surname() {
        return surname;
    }
};

class Student : virtual public Person {
    private:
    bool active = true;
    CoursesCollection courses;
    public:
    Student(std::string name, std::string surname, bool active = true) : Person(std::move(name), std::move(surname)) {
        this->active = active;
    }

    bool is_active() const {
        return active;
    }

    const CoursesCollection& get_courses() const {
        return courses;
    }
};

class Teacher : virtual public Person {
    private:
    CoursesCollection courses;
    public:
    Teacher(std::string name, std::string surname) : Person(std::move(name), std::move(surname)) {}

    virtual const CoursesCollection& get_courses() const {
        return courses;
    }
};

class PhDStudent : public Student, public Teacher {
    public:
    PhDStudent(std::string name, std::string surname, bool active = true) :
        Student(std::move(name), std::move(surname), active),
        Teacher(std::move(name), std::move(surname)) ,
        Person(name, surname)
        {};

    const CoursesCollection & get_courses() const override {
        return Teacher::get_courses();
    }
};

class Course {
private:
    std::string name;
    bool active = true;
public:
    Course(std::string name, bool active = true) {
        this->name = std::move(name);
        this->active = active;
    }

    std::string get_name() {
        return name;
    }

    bool is_active() {
        return active;
    }
};


class College {
    private:
    PeopleCollection people;
    CoursesCollection courses;
public:
    template <typename T>
    auto find(std::string name, std::string surname) {
        static_assert(std::is_base_of_v<Person, T>);
        std::vector<T*> result;

        for (const auto& person : people) {
            T* derivedPerson = dynamic_cast<T*>(person.get());
            if (derivedPerson != nullptr) {
                result.push_back(const_cast<T*>(derivedPerson));
            }
        }
        return result;
    }

//    template<typename T>
//    auto find(Course *course) {
//        return std::vector<T *>();
//    }

    bool add_course(std::string name, bool mandatory = true);

    std::vector<Course *> find_courses(const std::string& pattern);

    bool change_course_activeness(Course course, bool active);

    bool remove_course(Course course);

    bool change_student_activeness(Student student, bool active);

    template <typename T>
    bool add_person(std::string name, std::string surname, bool active = true);

    template <typename T>
    typename std::enable_if<std::is_base_of<T, Person>::value, bool>::type
    add_person(std::string name, std::string surname, bool active = true);


    template <typename T>
    bool assign_course(T person, Course course);
};




#endif //COLLEGE_COLLEGE_H
