#ifndef CONTACT_H
#define CONTACT_H
#include <string>
#include <cstring>

using namespace std;

class Contact
{
public:
    Contact();
    Contact(string id, string number, string country, string city, string address, string comment);

    void setPhoneNumber(string phoneNumber);
    string getPhoneNumber();

    void setCountry(string country);
    string getCountry();

    void setCity(string city);
    string getCity();

    void setAddress(string address);
    string getAddress();

    void setComment(string comment);
    string getComment();

    string setId();
    void setFileId(string id);
    string getId();

    ~Contact();
    friend class ContactRepository;

private:
    string _id;
    string _phoneNumber;
    string _country;
    string _city;
    string _address;
    string _comment;

};

#endif // CONTACT_H
