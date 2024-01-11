#include "college.h"

class GlupiKoledz {
private:
    PeopleCollection<Person> ludzie[3];

    void dodaj_inner(int gdzie, std::shared_ptr<Person> co) {
        ludzie[gdzie].emplace(co);
    }

    int policz_inner(int gdzie) {
        return ludzie[gdzie].size();
    }

    PeopleCollection<Person> kto_inner(int gdzie) {
        return ludzie[gdzie];
    }

public:
    template <CollegeMember T>
    void dodaj(std::string imie, std::string nazwisko, bool aktywnosc = true);

    template <PersonType T>
    int policz();

    template <PersonType T>
    PeopleCollection<T> kto();
};

template<>
void GlupiKoledz::dodaj<Student>(std::string imie, std::string nazwisko, bool aktywnosc) {
    std::shared_ptr<Student> ptr = std::make_shared<Student>(imie, nazwisko, aktywnosc);
    dodaj_inner(0, ptr);
}

template<>
void GlupiKoledz::dodaj<Teacher>(std::string imie, std::string nazwisko, bool aktywnosc) {
    std::shared_ptr<Teacher> ptr = std::make_shared<Teacher>(imie, nazwisko);
    dodaj_inner(1, ptr);
}

template<>
void GlupiKoledz::dodaj<PhDStudent>(std::string imie, std::string nazwisko, bool aktywnosc) {
    std::shared_ptr<PhDStudent> ptr = std::make_shared<PhDStudent>(imie, nazwisko, aktywnosc);
    dodaj_inner(2, ptr);
}


template<>
int GlupiKoledz::policz<Person>() {
    return policz_inner(0) + policz_inner(1) + policz_inner(2);
}

template<>
int GlupiKoledz::policz<Student>() {
    return policz_inner(0) + policz_inner(2);
}

template<>
int GlupiKoledz::policz<Teacher>() {
    return policz_inner(1) + policz_inner(2);
}

template<>
int GlupiKoledz::policz<PhDStudent>() {
    return policz_inner(2);
}

template<>
PeopleCollection<Person> GlupiKoledz::kto<Person>() {
    PeopleCollection<Person> ret = kto_inner(0);
    PeopleCollection<Person> tmp = kto_inner(1);
    for (auto i : tmp) {
        ret.emplace(i);
    }
    tmp = kto_inner(2);
    for (auto i : tmp) {
        ret.emplace(i);
    }
    return ret;
}

template<>
PeopleCollection<Student> GlupiKoledz::kto<Student>() {
    PeopleCollection<Student> ret;
    PeopleCollection<Person> tmp;
    for (int i : {0, 2}) {
        tmp = kto_inner(i);
        for (auto i : tmp) {
            ret.emplace(std::dynamic_pointer_cast<Student>(i));
        }
    }
    return ret;
}

template<>
PeopleCollection<Teacher> GlupiKoledz::kto<Teacher>() {
    PeopleCollection<Teacher> ret;
    PeopleCollection<Person> tmp;
    for (int i : {1, 2}) {
        tmp = kto_inner(i);
        for (auto i : tmp) {
            ret.emplace(std::dynamic_pointer_cast<Teacher>(i));
        }
    }
    return ret;
}

template<>
PeopleCollection<PhDStudent> GlupiKoledz::kto<PhDStudent>() {
    PeopleCollection<PhDStudent> ret;
    PeopleCollection<Person> tmp = kto_inner(2);
    for (auto i : tmp) {
        ret.emplace(std::dynamic_pointer_cast<PhDStudent>(i));
    }
    return ret;
}

int main() {
    GlupiKoledz kol;
    kol.dodaj<Student>("S", "1");
    kol.dodaj<Teacher>("T", "2");
    kol.dodaj<Teacher>("T", "3");
    kol.dodaj<PhDStudent>("P", "4");
    kol.dodaj<PhDStudent>("P", "5");
    kol.dodaj<PhDStudent>("P", "6");

    std::cout << kol.policz<PhDStudent>() << "\n"
    << kol.policz<Teacher>() << "\n"
    << kol.policz<Student>() << "\n"
    << kol.policz<Person>() << "\n";

    auto x = kol.kto<Person>();
    for (auto i : x) {
        std::cout << i->get_name() << " " << i->get_surname() << "\n";
    }
    std::cout << std::endl;

    auto y = kol.kto<Student>();
    for (auto i : y) {
        std::cout << i->get_name() << " " << i->get_surname() << " " << i->is_active() << "\n";
    }
    std::cout << std::endl;

    auto z = kol.kto<Teacher>();
    for (auto i : z) {
        std::cout << i->get_name() << " " << i->get_surname() /*<< " " << i->is_active()*/ << "\n";
    }
    std::cout << std::endl;

    auto z1 = kol.kto<PhDStudent>();
    for (auto i : z1) {
        std::cout << i->get_name() << " " << i->get_surname() << " " << i->is_active() << "\n";
    }
    std::cout << std::endl;

}