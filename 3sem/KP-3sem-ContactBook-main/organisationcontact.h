#ifndef ORGANISATIONCONTACT_H
#define ORGANISATIONCONTACT_H
#include <string>
#include <cstring>
#include <contact.h>

using namespace std;

class OrganisationContact: virtual public Contact {

public:
    OrganisationContact();

    OrganisationContact(string nameOrg);

    void setNameOrg(string nameOrg);

    string getNameOrg();

    ~OrganisationContact();

    friend class ContactRepository;
private:
    string _nameOrg;
};

#endif // ORGANISATIONCONTACT_H
