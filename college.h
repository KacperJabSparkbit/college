//
// Created by Kacper Jablonski on 24/12/2023.
//

#include <string>
#include <utility>
#include <regex>
#include <set>
#include <exception>

class Course;

class Person;

class Student;

class Teacher;

class PhDStudent;

class College;

template<typename T>
concept CollegeMemberNonPhD = std::is_same_v<Student, T> || std::is_same_v<Teacher, T>;

template<typename T>
concept CollegeMember = CollegeMemberNonPhD<T> || std::is_same_v<PhDStudent, T>;

template<typename T>
concept PersonType = CollegeMember<T> || std::is_same_v<Person, T>;


class PeopleComparator {
public:
    bool operator()(const std::weak_ptr<Person> &lhs, const std::weak_ptr<Person> &rhs) const;
};

template<PersonType T>
using PeopleCollection = std::set<std::shared_ptr<T>, PeopleComparator>;

template<PersonType T>
using WeakPeopleCollection = std::set<std::weak_ptr<T>, PeopleComparator>;

class CourseComparator {
public:
    bool operator()(const std::shared_ptr<Course> &lhs, const std::shared_ptr<Course> &rhs) const;
};

using CoursesCollection = std::set<std::shared_ptr<Course>, CourseComparator>;


class Course {
private:
    std::string name;
    bool active = true;
    College *college;
    WeakPeopleCollection<Student> students;
    WeakPeopleCollection<Teacher> teachers;

    void change_activeness(bool new_active);

public:
    Course(std::string name, bool active = true);

    std::string get_name() const;

    bool is_active() const;

    friend College;
};


class Person {
private:
    std::string name;
    std::string surname;
protected:
    College *college = nullptr;

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

    const CoursesCollection &get_courses() const;

    friend College;
};


class Teacher : virtual public Person {
private:
    CoursesCollection courses;

public:
    Teacher(std::string name, std::string surname);

    const CoursesCollection &get_courses() const;

    friend College;
};


class PhDStudent : public Student, public Teacher {
public:
    PhDStudent(std::string name, std::string surname, bool active = true);
};

class College {
private:
    PeopleCollection<Person> people[3];
    CoursesCollection courses;

    bool person_name_unique(const std::shared_ptr<Person> &person);

    bool try_add_person(int cointainer_index, const std::shared_ptr<Person> &person);

    template<PersonType T>
    void append_matching(PeopleCollection<T> &appendable, int containrer_index, const std::string &name_pattern,
                         const std::string &surname_pattern);

    template<CollegeMemberNonPhD T>
    bool assign_course_inner(const std::shared_ptr<Person> &person, const std::shared_ptr<Course> &course);

    void check_course(const std::shared_ptr<Course> &course);

    void check_course_existence(const std::shared_ptr<Course> &course);

    void check_course_activeness(const std::shared_ptr<Course> &course);

public:
    template<CollegeMember T>
    bool add_person(const std::string &name, const std::string &surname, bool active = true) {
        if (std::is_same_v<T, Teacher>) {
            std::shared_ptr<Person> added = std::make_shared<Teacher>(name, surname);
            return try_add_person(1, added);
        } else if (std::is_same_v<T, Student>) {
            std::shared_ptr<Person> added = std::make_shared<Student>(name, surname, active);
            return try_add_person(0, added);
        } else {
            std::shared_ptr<Person> added = std::make_shared<PhDStudent>(name, surname, active);
            return try_add_person(2, added);
        }
    }

    bool add_course(const std::string &name, bool mandatory = true);

    template<PersonType T>
    PeopleCollection<T> find(const std::string &name, const std::string &surname);

    template<CollegeMemberNonPhD T>
    WeakPeopleCollection<T> find(const std::shared_ptr<Course> &course);

    CoursesCollection find_courses(const std::string &pattern);

    bool change_course_activeness(const std::shared_ptr<Course> &course, bool active);

    bool remove_course(std::shared_ptr<Course> course);

    bool change_student_activeness(std::shared_ptr<Student> student, bool active);

    template<CollegeMemberNonPhD T>
    bool assign_course(const std::shared_ptr<T> &person, const std::shared_ptr<Course> &course);
};