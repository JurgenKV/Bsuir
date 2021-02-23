#ifndef PERSONALCONTACT_H
#define PERSONALCONTACT_H
#include <string>
#include <cstring>
#include <contact.h>

using namespace std;

class PersonalContact : virtual public Contact {

public:
    PersonalContact();
    PersonalContact(string firstname, string lastname);

    void setFirstname(string firstname);
    string getFirstname();

    void setLastname(string lastname);
    string getLastname();

    string getFullName();

    ~PersonalContact();

    friend class ContactRepository;
private:
    string _firstname;
    string _lastname;
};

#endif // PERSONALCONTACT_H
