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

class Course;
class Person;
class Student;
class Teacher;
class PhDStudent;


template <typename T>
concept PersonType = std::is_base_of_v<Person, T>;

template <typename T>
concept CollegeMember = std::is_base_of_v<Student, T> || std::is_base_of_v<Teacher, T>;

template <typename T>
concept CollegeMemberNonPhD = (std::is_base_of_v<Student, T> || std::is_base_of_v<Teacher, T>) && !std::is_same_v<PhDStudent, T>;




class Course {
private:
    std::string name;
    bool active = true;
public:
    Course(std::string name, bool active = true);
    std::string get_name() const;
    void change_activeness(bool new_active);
    bool is_active() const;
};


class CourseComparator {
public:
    CourseComparator() = default;
    bool operator()(const std::shared_ptr<Course>& lhs, const std::shared_ptr<Course>& rhs) const {
        return lhs->get_name() < rhs->get_name();
    }
};

using CoursesCollection = std::set<std::shared_ptr<Course>, CourseComparator>;


class Person {
private:
    std::string name;
    std::string surname;

protected:
public:
    Person(std::string name, std::string surname);
    std::string get_name() const;
    std::string get_surname() const;
    virtual ~Person() = default;
};

class PeopleComparator {
public:
    bool operator()(const std::shared_ptr<Person>& lhs, const std::shared_ptr<Person>& rhs) const {
        if (lhs->get_surname() == rhs->get_surname())
            return lhs->get_name() < rhs->get_name();
        return lhs->get_surname() < rhs->get_surname();
    }
};

template <PersonType T>
using PeopleCollection = std::set<std::shared_ptr<T>, PeopleComparator>;

class Student : virtual public Person {
private:
    bool active = true;
    CoursesCollection courses;

public:
    Student(std::string name, std::string surname, bool active = true);
    bool is_active() const;
    const CoursesCollection& get_courses() const;
    void change_activeness(bool activeness);
};

class Teacher : virtual public Person {
private:
    CoursesCollection courses;

public:
    Teacher(std::string name, std::string surname);
    const CoursesCollection& get_courses() const;
};

class PhDStudent : public Student, public Teacher {
public:
    PhDStudent(std::string name, std::string surname, bool active = true);
    const CoursesCollection& get_courses() const;
};

class College {
    private:
    PeopleCollection<Student> students;
    PeopleCollection<Teacher> teachers;
    PeopleCollection<PhDStudent> phd_students;
    CoursesCollection courses;

    bool person_exists(const std::string& name, const std::string& surname);

    bool course_exists(const std::string& name);

    Course* find_course(const std::string& name);

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