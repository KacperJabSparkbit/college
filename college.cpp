//
// Created by Kacper Jablonski on 24/12/2023.
//
#include "college.h"

namespace {
}


bool College::add_course(std::string name, bool mandatory) {
    if (course_exists(name))
        return false;
    courses.insert(std::make_shared<Course>(name, mandatory));
    return true;
}

