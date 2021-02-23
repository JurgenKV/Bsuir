#include "personalcontact.h"
#include "contact.h"
#include <iostream>
#include <clocale>
#include <cstring>
#include <contact.h>

using namespace std;

PersonalContact::PersonalContact() {
    _firstname = "";
    _lastname = "";
}

PersonalContact::PersonalContact(string firstname, string lastname) : _firstname(firstname), _lastname(lastname) {};

void PersonalContact::setFirstname(string firstname) {
    this->_firstname = firstname;
}
string PersonalContact::getFirstname() {
    return _firstname;
}

void PersonalContact::setLastname(string lastname) {
    this->_lastname = lastname;
}
string PersonalContact::getLastname() {
    return _lastname;
}

string PersonalContact::getFullName() {
    return this->_firstname + ' ' + this->_lastname;
}

PersonalContact::~PersonalContact() {};
