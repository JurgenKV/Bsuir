#include <iostream>
#include <clocale>
#include <cstring>
#include <string>
#include <contact.h>
#include <contactrepository.h>

using namespace std;

Contact::Contact()
{
    _id = setId();
    _phoneNumber = "";
    _country = "";
    _city = "";
    _address = "";
    _comment = "";
}

Contact::Contact(string id, string comment, string number, string country, string city, string address):
    _id(id), _phoneNumber(number), _country(country), _city(city), _address(address), _comment(comment) {};

void Contact::setPhoneNumber(string phoneNumber) {
    _phoneNumber = phoneNumber;
}
string Contact::getPhoneNumber() {
    return _phoneNumber;
}

void Contact::setCountry(string country) {
    this->_country = country;
}
string Contact::getCountry() {
    return _country;
}

void Contact::setCity(string city) {
    this->_city = city;
}
string Contact::getCity() {
    return _city;
}

void Contact::setAddress(string address) {
    this->_address = address;
}
string Contact::getAddress() {
    return _address;
}

void Contact::setComment(string comment) {
    this->_comment = comment;
}
string Contact::getComment() {
    return _comment;
}

string Contact::setId() {    
    while(1){
   ContactRepository Check ;
    string generateId;
    int a = rand() % 9999999999 + 1000000000;
    generateId = std::to_string(a);
    generateId[0] = 'C';
  bool cheaker =  Check.idCheck(generateId);
   if(cheaker == false)
       return generateId;
    }
}
void Contact::setFileId(string id){
    this->_id = id;
}

string Contact::getId() {
    return _id;
}


Contact::~Contact() {};
