#include "organisationcontact.h"
#include "contact.h"
#include <iostream>
#include <clocale>
#include <cstring>
#include <string>
#include <contact.h>

using namespace std;



OrganisationContact::OrganisationContact() {
    _nameOrg = "";
}

OrganisationContact::OrganisationContact(string nameOrg) :_nameOrg(nameOrg) {};

void OrganisationContact::setNameOrg(string nameOrg) {
    this->_nameOrg = nameOrg;
}
string OrganisationContact::getNameOrg() {
    return _nameOrg;
}

OrganisationContact::~OrganisationContact() {};
