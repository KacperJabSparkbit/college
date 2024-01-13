#include "college.h"

bool PeopleComparator::operator()(const std::shared_ptr<Person>& lhs, const std::shared_ptr<Person>& rhs) const {
    if (lhs->get_surname() == rhs->get_surname())
        return lhs->get_name() < rhs->get_name();
    return lhs->get_surname() < rhs->get_surname();
}

bool CourseComparator::operator()(const std::shared_ptr<Course>& lhs, const std::shared_ptr<Course>& rhs) const {
    return lhs->get_name() < rhs->get_name();
}


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



//College:
bool College::person_name_unique(std::shared_ptr<Person> person) {
    for (int i : {0, 1, 2}) {
        if (people[i].contains(person)) {
            return false;
        }
    }
    return true;
}


bool College::try_add_person(int cointainer_index, std::shared_ptr<Person> person) {
    if (person_name_unique(person)) {
        people[cointainer_index].emplace(person);
        person->college = this;
        return true;
    } else {
        return false;
    }
}

template<>
bool College::add_person<Student>(std::string name, std::string surname, bool activeness) {
    std::shared_ptr<Person> added = std::make_shared<Student>(name, surname, activeness);
    return try_add_person(0, added);
}

template<>
bool College::add_person<Teacher>(std::string name, std::string surname, bool activeness) {
    std::shared_ptr<Person> added = std::make_shared<Teacher>(name, surname);
    return try_add_person(1, added);
}

template<>
bool College::add_person<PhDStudent>(std::string name, std::string surname, bool activeness) {
    std::shared_ptr<Person> added = std::make_shared<PhDStudent>(name, surname, activeness);
    return try_add_person(2, added);
}


bool College::add_course(std::string name, bool activeness) {
    std::shared_ptr<Course> added = std::make_shared<Course>(name, activeness);
    added->college = this;
    return courses.emplace(added).second;
}


//na razie wyszukuje po pelnym imieniu i nazwisku i liniowo
template<PersonType T>
void College::append_matching(PeopleCollection<T>& appendable, int containrer_index, std::string name_pattern, std::string surname_pattern) {
    for (auto i : people[containrer_index]) {
        if (wildcardMatch(name_pattern, i->get_name()) && wildcardMatch(surname_pattern, i->get_surname())) {
            appendable.emplace(std::dynamic_pointer_cast<T>(i));
        }
    }
}

template<>
PeopleCollection<Person> College::find<Person>(std::string name, std::string surname) {
    PeopleCollection<Person> matches;
    append_matching(matches, 0, name, surname);
    append_matching(matches, 1, name, surname);
    append_matching(matches, 2, name, surname);
    return(matches);
}

template<>
PeopleCollection<Student> College::find<Student>(std::string name, std::string surname) {
    PeopleCollection<Student> matches;
    append_matching(matches, 0, name, surname);
    append_matching(matches, 2, name, surname);
    return(matches);
}

template<>
PeopleCollection<Teacher> College::find<Teacher>(std::string name, std::string surname) {
    PeopleCollection<Teacher> matches;
    append_matching(matches, 1, name, surname);
    append_matching(matches, 2, name, surname);
    return(matches);
}

template<>
PeopleCollection<PhDStudent> College::find<PhDStudent>(std::string name, std::string surname) {
    PeopleCollection<PhDStudent> matches;
    append_matching(matches, 2, name, surname);
    return(matches);
}

//again nie wyszukuje po patternie
CoursesCollection College::find_courses(const std::string& pattern) {
    CoursesCollection matching;
    for (auto i : courses) {
        if (wildcardMatch(pattern, i->get_name())) {
            matching.emplace(i);
        }
    }
    return matching;
}


template<>
PeopleCollection<Student> College::find<Student>(std::shared_ptr<Course> course) {
    if (course->college != this) {
        throw std::runtime_error("Non-existing course.");
    }
    return course->students;
}

template<>
PeopleCollection<Teacher> College::find<Teacher>(std::shared_ptr<Course> course) {
    if (course->college != this) {
        throw std::runtime_error("Non-existing course.");
    }
    return course->teachers;
}

bool College::change_course_activeness(std::shared_ptr<Course> course, bool active) {
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

    //
    //for (auto i : course->students) {
    //    i->courses.erase(course);
    //}

    //for (auto i : course->teachers) {
    //    i->courses.erase(course);
    //}

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
bool College::assign_course_inner(std::shared_ptr<Person> person, std::shared_ptr<Course> course) {
    if (person->college != this) {
        throw std::runtime_error("Non-existing person.");
    }
    if (person->college != this || course->college != this || !course->is_active()) {
        throw std::exception();
    }
    course->all_assigned.emplace(person);
    if constexpr (std::is_same_v<Student, T>) {
        auto t = std::dynamic_pointer_cast<Student>(person);
        if (!t->is_active()) {
            throw std::runtime_error("Incorrect operation on an inactive student");
        }
        std::dynamic_pointer_cast<Student>(person)->courses.emplace(course);
        return course->students.emplace(std::dynamic_pointer_cast<Student>(person)).second;
    } else {
        std::dynamic_pointer_cast<Teacher>(person)->courses.emplace(course);
        return course->teachers.emplace(std::dynamic_pointer_cast<Teacher>(person)).second;
    }
    return true;
}

template<>
bool College::assign_course<Student>(std::shared_ptr<Student> person, std::shared_ptr<Course> course) {
    if (course->college != this) {
        throw std::runtime_error("Non-existing course.");
    }
    if (!course->is_active()) {
        throw std::runtime_error("Incorrect operation on an inactive course");
    }
    return assign_course_inner<Student>(person, course);
}

template<>
bool College::assign_course<Teacher>(std::shared_ptr<Teacher> person, std::shared_ptr<Course> course) {
    if (course->college != this) {
        throw std::runtime_error("Non-existing course.");
    }
    if (!course->is_active()) {
        throw std::runtime_error("Incorrect operation on an inactive course");
    }
    return assign_course_inner<Teacher>(person, course);
}
