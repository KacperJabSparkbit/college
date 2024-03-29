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
#include <map>

class Course;
class Person;
class Student;
class Teacher;
class PhDStudent;
class PeopleComparator;

template <typename T>
concept PersonType = std::is_base_of_v<Person, T>;

namespace {
    bool wildcardMatch(const std::string& pattern, const std::string& text) {
        std::regex star_replace("\\*");
        std::regex questionmark_replace("\\?");
        std::regex regexPattern("([.+^=!:${}()|\\[\\]\\/\\\\])");

        auto pattern_escaped = regex_replace(pattern, regexPattern, "\\$1");

        auto wildcard_pattern = regex_replace(
                regex_replace(pattern_escaped, star_replace, ".*"),
                questionmark_replace, ".");

        std::regex wildcard_regex(wildcard_pattern);
        return regex_match(text, wildcard_regex);
    }

}

class Course {
    friend class College;
private:
    std::string name;
    bool active = true;
    void change_activeness(bool new_active) {
        this->active = new_active;
    }
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

namespace {
    class CourseComparator {
    public:
        CourseComparator() = default;
        bool operator()(const std::shared_ptr<Course>& lhs, const std::shared_ptr<Course>& rhs) const {
            return lhs->get_name() < rhs->get_name();
        }
    };
}

using CoursesCollection = std::set<std::shared_ptr<Course>, CourseComparator>;

//interface for getting courses
class CollegePerson {
    virtual const CoursesCollection& get_courses() const = 0;
};

class Person {
    private:
    std::string name;
    std::string surname;

protected:
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

class Student : virtual public Person, virtual public CollegePerson {
    friend class College;
    private:
    bool active = true;
    CoursesCollection courses;
    protected:
    void change_activeness(bool activeness) {
        active = activeness;
    }
    void add_course(const std::shared_ptr<Course>& course) {
        courses.insert(course);
    }
    public:
    Student(std::string name, std::string surname, bool active = true) :
        Person(std::move(name), std::move(surname)) {
        this->active = active;
    }

    bool is_active() const {
        return active;
    }

    const CoursesCollection& get_courses() const override {
        return courses;
    }
};

class Teacher : virtual public Person, virtual public CollegePerson {
    friend class College;
    private:
    CoursesCollection courses;
    void add_course(const std::shared_ptr<Course>& course) {
        courses.insert(course);
    }
    public:
    Teacher(std::string name, std::string surname) :
        Person(std::move(name), std::move(surname)) {}

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

    const CoursesCollection& get_courses() const override {
        return Teacher::get_courses();
    }
};

class PeopleComparator {
public:
    bool operator()(const std::shared_ptr<Person>& lhs, const std::shared_ptr<Person>& rhs) const {
        if (lhs->get_surname() == rhs->get_surname())
            return lhs->get_name() < rhs->get_name();
        return lhs->get_surname() < rhs->get_surname();
    }
};

using PeopleCollection = std::set<std::shared_ptr<Person>, PeopleComparator>;


class College {
    friend class Student;
    friend class Course;
    private:
    PeopleCollection people;
    CoursesCollection courses;

    std::map<std::shared_ptr<Course>, PeopleCollection> course_to_student;
    std::map<std::shared_ptr<Course>, PeopleCollection> course_to_teacher;

    bool person_exists(const std::string& name, const std::string& surname) {
        for (auto const &p : people) {
            if (p->get_name() == name && p->get_surname() == surname)
                return true;
        }
        return false;
    }

    bool course_exists(const std::string& name) {
        for (auto const &c : courses) {
            if (c->get_name() == name)
                return true;
        }
        return false;
    }

    std::shared_ptr<Course> find_course(const std::string& name) {
        for (auto const &c : courses) {
            if (c->get_name() == name)
                return c;
        }
        return nullptr;
    }

    template <typename T>
    std::shared_ptr<T> _find(std::string name, std::string surname) {
        static_assert(std::is_base_of_v<Person, T>);
        for (const auto& person : people) {
            auto n = person->get_name();
            auto s = person->get_surname();
            if (n == name && s == surname) {
//                T* derivedPerson = dynamic_cast<T*>(person.get());
//                if (derivedPerson != nullptr) {
//                    return person;
//                }
                return std::dynamic_pointer_cast<T>(person);
            }
        }
        return nullptr;
    }

public:
    template <typename T>
    auto find(std::string name, std::string surname) {
        static_assert(std::is_base_of_v<Person, T>);
        std::vector<std::shared_ptr<T>> result;
        for (auto person : people) {
            auto n = person->get_name();
            auto s = person->get_surname();
            if (wildcardMatch(name, n) && wildcardMatch(surname, s)) {
                T* derivedPerson = dynamic_cast<T*>(person.get());
                if (derivedPerson != nullptr) {
                    result.push_back(std::dynamic_pointer_cast<T>(person));
                }
            }
        }
        return result;
    }

    template <typename T>
    auto find(std::shared_ptr<Course> course) {
        static_assert(std::is_base_of_v<Person, T>);
        std::vector<T*> result;
        auto c = find_course(course->get_name());
        if (c == nullptr)
            return result;
        if constexpr (std::is_same_v<T, Student>) {
            for (const auto& person : course_to_student[c]) {
                T* derivedPerson = dynamic_cast<T*>(person.get());
                if (derivedPerson != nullptr) {
                    result.push_back(const_cast<T*>(derivedPerson));
                }
            }
        }
        else if constexpr (std::is_same_v<T, Teacher>) {
            for (const auto& person : course_to_teacher[c]) {
                T* derivedPerson = dynamic_cast<T*>(person.get());
                if (derivedPerson != nullptr) {
                    result.push_back(const_cast<T*>(derivedPerson));
                }
            }
        }
        return result;
    }

    bool add_course(std::string name, bool mandatory = true);

    CoursesCollection find_courses(const std::string& pattern) {
        CoursesCollection result;
        for (const auto& course : courses) {
            if (wildcardMatch(pattern, course->get_name())) {
                result.insert(course);
            }
        }
        return result;
    }

    bool change_course_activeness(std::shared_ptr<Course> course, bool active) {
        if (!course_exists(course->get_name()))
            return false;
        course->change_activeness(active);
        return true;
    }

    bool remove_course(std::shared_ptr<Course> course) {
        if (!course_exists(course->get_name()))
            return false;
        course->change_activeness(false);
        courses.erase(course);
        return true;
    }

    bool change_student_activeness(std::shared_ptr<Student> student, bool active) {
        if (!person_exists(student->get_name(), student->get_surname()))
            return false;
        student->change_activeness(active);
        return true;
    }

    template <PersonType T>
    bool add_person(std::string name, std::string surname, bool active = true) {
        if (person_exists(name, surname))
            return false;
        if (std::is_same_v<T, Teacher>) {
            people.insert(std::make_shared<Teacher>(name, surname));
            return true;
        } else if (std::is_same_v<T, Student>) {
            people.insert(std::make_shared<Student>(name, surname, active));
            return true;
        } else {
            people.insert(std::make_shared<PhDStudent>(name, surname, active));
            return true;
        }
    }

    template <typename T>
    bool assign_course(std::shared_ptr<T> person, std::shared_ptr<Course> course){
        auto c = find_course(course->get_name());
        if (c == nullptr || c != course)
            throw std::runtime_error("Non-existing course.");
        if (!(*c).is_active()) {
            throw std::runtime_error("Incorrect operation on an inactive course");
        }
        if constexpr (std::is_same_v<T, Student>) {
            auto t = _find<Student>(person->get_name(), person->get_surname());
            if (t == nullptr || t != person)
                throw std::runtime_error("Non-existing person.");
            if ((*t).is_active() == false)
                throw std::runtime_error("Incorrect operation for an inactive student");
            course_to_student[c].insert(t);
            (*t).Student::add_course(c);
            return true;
        } else if constexpr (std::is_same_v<T, Teacher>) {
            auto t = _find<Teacher>(person->get_name(), person->get_surname());
            if (t == nullptr || t != person)
                throw std::runtime_error("Non-existing person.");
            course_to_teacher[c].insert(t);
            (*t).Teacher::add_course(c);
            return true;
        }
    }

    template <typename T>
    bool assign_course(std::shared_ptr<PhDStudent> person,  std::shared_ptr<Course> course){
        if constexpr (std::is_same_v<T, Student>) {
            return assign_course((std::shared_ptr<Student>)person, course);
        } else if constexpr (std::is_same_v<T, Teacher>) {
            return assign_course((std::shared_ptr<Teacher>)person, course);
        }
    }
};




#endif //COLLEGE_COLLEGE_H
