#include "college.h"
#include <limits>

bool PeopleComparator::operator()(const std::weak_ptr<Person> &lhs, const std::weak_ptr<Person> &rhs) const {
    auto slhs = lhs.lock();
    auto srhs = rhs.lock();
    if (slhs->get_surname() == srhs->get_surname())
        return slhs->get_name() < srhs->get_name();
    return slhs->get_surname() < srhs->get_surname();
}

bool CourseComparator::operator()(const std::shared_ptr<Course> &lhs, const std::shared_ptr<Course> &rhs) const {
    return lhs->get_name() < rhs->get_name();
}


namespace {
bool wildcardMatch(const std::string &pattern, const std::string &text) {
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

bool find_course_by_name(const CoursesCollection &courses, const std::string &name) {
    for (const auto &i: courses) {
        if (i->get_name() == name) {
            return true;
        }
    }
    return false;
}
}

//Course:
Course::Course(std::string name, bool active) {
    this->name = std::move(name);
    this->active = active;
    this->college = nullptr;
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

const CoursesCollection &Student::get_courses() const {
    return courses;
}

void Student::change_activeness(bool activeness) {
    active = activeness;
}

//Teacher:
Teacher::Teacher(std::string name, std::string surname) :
        Person(std::move(name), std::move(surname)) {}

const CoursesCollection &Teacher::get_courses() const {
    return courses;
}

//PhDStudent:
PhDStudent::PhDStudent(std::string name, std::string surname, bool active) :
        Person(name, surname),
        Student(std::move(name), std::move(surname), active),
        Teacher(std::move(name), std::move(surname))
{};

//College:
bool College::person_name_unique(const std::shared_ptr<Person> &person) {
    for (int i: {0, 1, 2}) {
        if (people[i].contains(person)) {
            return false;
        }
    }
    return true;
}

bool College::try_add_person(int cointainer_index, const std::shared_ptr<Person> &person) {
    if (person_name_unique(person)) {
        people[cointainer_index].emplace(person);
        person->college = this;
        return true;
    } else {
        return false;
    }
}

bool College::add_course(const std::string &name, bool activeness) {
    std::shared_ptr<Course> added = std::make_shared<Course>(name, activeness);
    added->college = this;
    return courses.insert(added).second;
}

template<PersonType T>
void College::append_matching(PeopleCollection<T> &appendable, int containrer_index, const std::string &name_pattern,
                              const std::string &surname_pattern) {
    if (name_pattern.find_first_of("*?") == std::string::npos &&
        surname_pattern.find_first_of("*?") == std::string::npos) {
        auto iter = people[containrer_index].find(std::make_shared<Person>(name_pattern, surname_pattern));
        if (iter != people[containrer_index].end()) {
            appendable.emplace(std::dynamic_pointer_cast<T>(*iter));
        }
        return;
    }

    for (auto i: people[containrer_index]) {
        if (wildcardMatch(name_pattern, i->get_name()) && wildcardMatch(surname_pattern, i->get_surname())) {
            appendable.emplace(std::dynamic_pointer_cast<T>(i));
        }
    }
}

template<>
PeopleCollection<Person> College::find<Person>(const std::string &name, const std::string &surname) {
    PeopleCollection<Person> matches;
    append_matching(matches, 0, name, surname);
    append_matching(matches, 1, name, surname);
    append_matching(matches, 2, name, surname);
    return (matches);
}

template<>
PeopleCollection<Student> College::find<Student>(const std::string &name, const std::string &surname) {
    PeopleCollection<Student> matches;
    append_matching(matches, 0, name, surname);
    append_matching(matches, 2, name, surname);
    return (matches);
}

template<>
PeopleCollection<Teacher> College::find<Teacher>(const std::string &name, const std::string &surname) {
    PeopleCollection<Teacher> matches;
    append_matching(matches, 1, name, surname);
    append_matching(matches, 2, name, surname);
    return (matches);
}

template<>
PeopleCollection<PhDStudent> College::find<PhDStudent>(const std::string &name, const std::string &surname) {
    PeopleCollection<PhDStudent> matches;
    append_matching(matches, 2, name, surname);
    return (matches);
}

CoursesCollection College::find_courses(const std::string &pattern) {
    CoursesCollection matching;

    if (pattern.find_first_of("*?") == std::string::npos) {
        auto iter = courses.find(std::make_shared<Course>(pattern));
        if (iter != courses.end()) {
            matching.emplace(*iter);
        }
        return matching;
    }

    for (const auto &i: courses) {
        if (wildcardMatch(pattern, i->get_name())) {
            matching.emplace(i);
        }
    }
    return matching;
}


template<>
WeakPeopleCollection<Student> College::find<Student>(const std::shared_ptr<Course> &course) {
    if (course->college != this) {
        return WeakPeopleCollection<Student>();
    }
    return course->students;
}

template<>
WeakPeopleCollection<Teacher> College::find<Teacher>(const std::shared_ptr<Course> &course) {
    if (course->college != this) {
        return WeakPeopleCollection<Teacher>();
    }
    return course->teachers;
}

bool College::change_course_activeness(const std::shared_ptr<Course> &course, bool active) {
    if (course->college != this) {
        return false;
    }
    course->change_activeness(active);
    return true;
}

bool College::remove_course(std::shared_ptr<Course> course) {
    if (course->college != this) {
        return false;
    }
    course->change_activeness(false);
    course->college = nullptr;
    courses.erase(course);

    return true;
}

bool College::change_student_activeness(std::shared_ptr<Student> student, bool active) {
    if (student->college != this) {
        return false;
    }
    student->change_activeness(active);
    return true;
}

template<CollegeMemberNonPhD T>
bool College::assign_course_inner(const std::shared_ptr<Person> &person, const std::shared_ptr<Course> &course) {
    if (person->college != this) {
        throw std::runtime_error("Non-existing person.");
    }
    check_course(course);
    if constexpr (std::is_same_v<Student, T>) {
        std::dynamic_pointer_cast<Student>(person);
        if (!(std::dynamic_pointer_cast<Student>(person))->is_active()) {
            throw std::runtime_error("Incorrect operation for an inactive student.");
        }
        auto studentCourses = std::dynamic_pointer_cast<Student>(person)->get_courses();
        if (find_course_by_name(studentCourses, course->get_name())) {
            return false;
        }
        std::dynamic_pointer_cast<Student>(person)->courses.insert(course);
        return course->students.insert(std::dynamic_pointer_cast<Student>(person)).second;
    } else {
        auto teacherCourses = std::dynamic_pointer_cast<Teacher>(person)->get_courses();
        if (find_course_by_name(teacherCourses, course->get_name())) {
            return false;
        }
        std::dynamic_pointer_cast<Teacher>(person)->courses.insert(course);
        return course->teachers.insert(std::dynamic_pointer_cast<Teacher>(person)).second;
    }
}

void College::check_course(const std::shared_ptr<Course> &course) {
    check_course_existence(course);
    check_course_activeness(course);
}

void College::check_course_activeness(const std::shared_ptr<Course> &course) {
    if (!course->is_active()) {
        throw std::runtime_error("Incorrect operation on an inactive course.");
    }
}

void College::check_course_existence(const std::shared_ptr<Course> &course) {
    if (course->college != this) {
        throw std::runtime_error("Non-existing course.");
    }
}

template<>
bool College::assign_course<Student>(const std::shared_ptr<Student> &person, const std::shared_ptr<Course> &course) {
    check_course(course);
    return assign_course_inner<Student>(person, course);
}

template<>
bool College::assign_course<Teacher>(const std::shared_ptr<Teacher> &person, const std::shared_ptr<Course> &course) {
    check_course(course);
    return assign_course_inner<Teacher>(person, course);
}