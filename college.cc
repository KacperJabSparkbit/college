#include "college.h"

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




} //namespace



//Course:
Course::Course(std::string name, bool active) {
    this->name = std::move(name);
    this->active = active;
}

std::string Course::get_name() const {
    return name;
}

void Course::change_activeness(bool new_active) {
    this->active = new_active;
}

bool Course::is_active() const {
    return active;
}


//Person:
Person::Person(std::string name, std::string surname) {
    this->name = std::move(name);
    this->surname = std::move(surname);
}


std::string Person::get_name() const {
    return name;
}

std::string Person::get_surname() const {
    return surname;
}


//Student:
Student::Student(std::string name, std::string surname, bool active) :
    Person(std::move(name), std::move(surname)) {
    this->active = active;
}

bool Student::is_active() const {
    return active;
}

const CoursesCollection& Student::get_courses() const {
    return courses;
}

void Student::change_activeness(bool activeness) {
    active = activeness;
}

//Teacher:
Teacher::Teacher(std::string name, std::string surname) :
    Person(std::move(name), std::move(surname)) {}

const CoursesCollection& Teacher::get_courses() const {
    return courses;
}



//PhDStudent:
PhDStudent::PhDStudent(std::string name, std::string surname, bool active) :
    Student(std::move(name), std::move(surname), active),
    Teacher(std::move(name), std::move(surname)) ,
    Person(name, surname)
    {};

const CoursesCollection& PhDStudent::get_courses() const {
    return Teacher::get_courses();
}
