//
// Created by Kacper Jablonski on 24/12/2023.
//

#ifndef COLLEGE_COLLEGE_H
#define COLLEGE_COLLEGE_H

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <regex>
#include <set>
#include <assert.h> //
#include <map>
#include <exception>

class Course;
class Person;
class Student;
class Teacher;
class PhDStudent;
class College;


template <typename T>
concept CollegeMemberNonPhD = std::is_same_v<Student, T> || std::is_same_v<Teacher, T>;

template <typename T>
concept CollegeMember = CollegeMemberNonPhD<T> || std::is_same_v<PhDStudent, T>;

template <typename T>
concept PersonType = CollegeMember<T> || std::is_same_v<Person, T>;


class PeopleComparator {
public:
    bool operator()(const std::shared_ptr<Person>& lhs, const std::shared_ptr<Person>& rhs) const;
};
template <PersonType T>
using PeopleCollection = std::set<std::shared_ptr<T>, PeopleComparator>;

class CourseComparator {
public:
    bool operator()(const std::shared_ptr<Course>& lhs, const std::shared_ptr<Course>& rhs) const;
};
using CoursesCollection = std::set<std::shared_ptr<Course>, CourseComparator>;




class Course {
private:
    std::string name;
    bool active = true;
    College* college;
    //PeopleCollection<Person> all_assigned;
    PeopleCollection<Student> students;
    PeopleCollection<Teacher> teachers;

public:
    Course(std::string name, bool active = true);
    std::string get_name() const;
    void change_activeness(bool new_active);
    bool is_active() const;

    friend College;
};


class Person {
private:
    std::string name;
    std::string surname;
protected:
    College* college = nullptr;

public:
    Person(std::string name, std::string surname);
    std::string get_name() const;
    std::string get_surname() const;
    virtual ~Person() = default;

    friend College;
};







class Student : virtual public Person {
private:
    bool active = true;
    CoursesCollection courses;
protected:
    void change_activeness(bool activeness);

public:
    Student(std::string name, std::string surname, bool active = true);
    bool is_active() const;
    const CoursesCollection& get_courses() const;

    friend College;
};

class Teacher : virtual public Person {
private:
    CoursesCollection courses;

public:
    Teacher(std::string name, std::string surname);
    const CoursesCollection& get_courses() const;

    friend College;
};

class PhDStudent : public Student, public Teacher {
public:
    PhDStudent(std::string name, std::string surname, bool active = true);
    //const CoursesCollection& get_courses() const;
};

class College {
    private:

    PeopleCollection<Person> people[3];
    CoursesCollection courses;

    bool person_name_unique(std::shared_ptr<Person> person);
    bool try_add_person(int cointainer_index, std::shared_ptr<Person> person);

    template<PersonType T>
    void append_matching(PeopleCollection<T>& appendable, int containrer_index, std::string name_pattern, std::string surname_pattern);

    template<CollegeMemberNonPhD T>
    bool assign_course_inner(std::shared_ptr<Person> person, std::shared_ptr<Course> course);

public:
    template <CollegeMember T>
    bool add_person(std::string name, std::string surname, bool active = true);

    bool add_course(std::string name, bool mandatory = true);
    
    
    template <PersonType T>
    PeopleCollection<T> find(std::string name, std::string surname);

    template <CollegeMemberNonPhD T>
    PeopleCollection<T> find(std::shared_ptr<Course> course);

    CoursesCollection find_courses(const std::string& pattern);


    bool change_course_activeness(std::shared_ptr<Course> course, bool active);

    bool remove_course(std::shared_ptr<Course> course);

    bool change_student_activeness(std::shared_ptr<Student> student, bool active);


    template <CollegeMemberNonPhD T>
    bool assign_course(std::shared_ptr<T> person, std::shared_ptr<Course> course);
};



#endif