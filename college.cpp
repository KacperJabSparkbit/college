//
// Created by Kacper Jablonski on 24/12/2023.
//
#include "college.h"

namespace {
    bool person_exists(const std::vector<std::shared_ptr<Person>>& people, const std::string& name, const std::string& surname) {
        for (auto const &p : people) {
            if (p->get_name() == name && p->get_surname() == surname)
                return true;
        }
        return false;
    }

    bool course_exists(const std::vector<std::shared_ptr<Course>>& courses, const std::string& name) {
        for (auto const &c : courses) {
            if (c->get_name() == name)
                return true;
        }
        return false;
    }
}

template <typename T>
//typename std::enable_if<std::is_base_of<Person, T>::value, bool>::type
bool College::add_person(std::string name, std::string surname, bool active) {
    static_assert(std::is_base_of_v<Person, T>);
    if (person_exists(people, name, surname))
        return false;
    people.emplace_back(std::make_shared<T>(name, surname, active));
    return true;
}

bool College::add_course(std::string name, bool mandatory) {
    if (course_exists(courses, name))
        return false;
    courses.emplace_back(std::make_shared<Course>(name, mandatory));
    return true;
}

std::vector<Course *> College::find_courses(const std::string& pattern) {
    return std::vector<Course *>();
}
