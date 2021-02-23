#include <contactrepository.h>
#include <contact.h>
#include <iostream>
#include <clocale>
#include <string>
#include <cstring>
#include <vector>
#include <personalcontact.h>
#include <organisationcontact.h>
#include <iomanip>
#include <fstream>

void ContactRepository::CreateContact(Contact a)
{
    this->vector.push_back(a);
}
void ContactRepository::CreateOrgContact(OrganisationContact a){
    this->vectorOrgContact.push_back(a);
}

void ContactRepository::CreatePerContact(PersonalContact a){
    this->vectorPerContact.push_back(a);
}

int ContactRepository::getVecSizePerContact(){
    return vectorPerContact.size();
}

int ContactRepository::getVecSizeOrgContact(){
    return vectorOrgContact.size();
}

int ContactRepository::getVecSizeContact()
{
    return ( getVecSizePerContact() + getVecSizeOrgContact() );
}

string ContactRepository::numVecId(unsigned long long a)
{
    return vector[a].getId();
}

string ContactRepository::searchContact(string search)
{
    for (unsigned long long i = 0; i < vectorOrgContact.size(); i++)
    {
        if ((search == vectorOrgContact[i].getPhoneNumber() ))
        {
            return vectorOrgContact[i].getId();
        }
    }

    for (unsigned long long i = 0; i < vectorPerContact.size(); i++)
    {
        if ((search == vectorPerContact[i].getPhoneNumber() ))
        {
            return vectorPerContact[i].getId();
        }
    }
    return "";
}

string ContactRepository::searchContactToUpdate(string search)
{
    for (unsigned long long i = 0; i < vectorOrgContact.size(); i++)
    {
        if ((search == vectorOrgContact[i].getPhoneNumber() ))
        {
            return search;
        }
    }

    for (unsigned long long i = 0; i < vectorPerContact.size(); i++)
    {
        if ((search == vectorPerContact[i].getPhoneNumber() ))
        {
            return search;
        }
    }
    return "";
}

bool ContactRepository::searchContactNumberOrg(string search)
{
    for (unsigned long long i = 0; i < vectorOrgContact.size(); i++)
    {
        if ((search == vectorOrgContact[i].getPhoneNumber() ))
        {
            return true;
        }
    }
     return false;
}

 bool ContactRepository::searchContactNumberPer(string search)
{
    for (unsigned long long i = 0; i < vectorPerContact.size(); i++)
    {
        if ((search == vectorPerContact[i].getPhoneNumber() ))
        {
            return true;
        }
    }
   return false;
}


PersonalContact ContactRepository::PerContactToTable(int i)
{
    return vectorPerContact[i];
}

OrganisationContact ContactRepository::OrgContactToTable(int i)
{
    return vectorOrgContact[i];
}


PersonalContact ContactRepository::searchPerContact(string search, unsigned long long k)
{
    for (; k < vectorPerContact.size(); k++)
    {
        if ((search == vectorPerContact[k].getAddress()) || (search == vectorPerContact[k].getCity()) || (search == vectorPerContact[k].getCountry()) ||
                (search == vectorPerContact[k].getPhoneNumber() || (search == vectorPerContact[k].getFirstname()) ||
                 (search == vectorPerContact[k].getLastname()) || (search == vectorPerContact[k].getFullName()) ))
        {
            return vectorPerContact[k];
        }
    }
}

unsigned long long ContactRepository::searchPerContactPosition(string search, unsigned long long k)
{
    for (; k < vectorPerContact.size(); k++)
    {
        if ((search == vectorPerContact[k].getAddress()) || (search == vectorPerContact[k].getCity()) || (search == vectorPerContact[k].getCountry()) ||
                (search == vectorPerContact[k].getPhoneNumber() || (search == vectorPerContact[k].getFirstname()) ||
                 (search == vectorPerContact[k].getLastname()) || (search == vectorPerContact[k].getFullName()) ))
        {
            return k;
        }
    }
}

bool ContactRepository::searchPerContactBool(string search, unsigned long long i)
{
    for (; i < vectorPerContact.size(); i++)
    {
        if ((search == vectorPerContact[i].getAddress()) || (search == vectorPerContact[i].getCity()) || (search == vectorPerContact[i].getCountry()) ||
                (search == vectorPerContact[i].getPhoneNumber() || (search == vectorPerContact[i].getFirstname()) ||
                 (search == vectorPerContact[i].getLastname()) || (search == vectorPerContact[i].getFullName()) ))
        {
            return true;
        }
    }
    return false;
}

OrganisationContact ContactRepository::searchOrgContact(string search, unsigned long long i)
{
    for (; i < vectorOrgContact.size(); i++)
    {
        if ((search == vectorOrgContact[i].getAddress()) || (search == vectorOrgContact[i].getCity()) || (search == vectorOrgContact[i].getCountry()) ||
                (search == vectorOrgContact[i].getPhoneNumber() || (search == vectorOrgContact[i].getNameOrg()) ))
        {
            return vectorOrgContact[i];
        }
    }
}

unsigned long long ContactRepository::searchOrgContactPosition(string search, unsigned long long i)
{
    for (; i < vectorOrgContact.size(); i++)
    {
        if ((search == vectorOrgContact[i].getAddress()) || (search == vectorOrgContact[i].getCity()) || (search == vectorOrgContact[i].getCountry()) ||
                (search == vectorOrgContact[i].getPhoneNumber() || (search == vectorOrgContact[i].getNameOrg()) ))
        {
            return i;
        }
    }
}

bool ContactRepository::searchOrgContactBool(string search, unsigned long long i)
{
    for (; i < vectorOrgContact.size(); i++)
    {
        if ((search == vectorOrgContact[i].getAddress()) || (search == vectorOrgContact[i].getCity()) || (search == vectorOrgContact[i].getCountry()) ||
                (search == vectorOrgContact[i].getPhoneNumber() || (search == vectorOrgContact[i].getNameOrg()) ))
        {
            return true;
        }
    }
    return false;
}

void ContactRepository::Delete(string id, int categoty)
{
    unsigned long long delNum;
    if(categoty == 0){
        for (unsigned long long i = 0; i < vectorOrgContact.size(); i++)
        {
            if (id == vectorOrgContact[i].getId())
            {
                delNum = i;
                vectorOrgContact.erase(vectorOrgContact.begin()+delNum);
                return;
            }
        }
    }
    if(categoty == 1){
        for (unsigned long long i = 0; i < vectorPerContact.size(); i++)
        {
            if (id == vectorPerContact[i].getId())
            {
                delNum = i;
                vectorPerContact.erase(vectorPerContact.begin()+delNum);
                return;
            }
        }
    }
}

bool ContactRepository::idCheck(string id)
{
    for (unsigned long long i = 0; i < vectorOrgContact.size(); i++)
    {
        if (id == vectorOrgContact[i].getId())
        {
            return true;
        }
    }
    for (unsigned long long i = 0; i < vectorPerContact.size(); i++)
    {
        if (id == vectorPerContact[i].getId())
        {
            return true;
        }
    }

    return false;

}

PersonalContact ContactRepository::getPerObj(string id)
{
    for (unsigned long long i = 0; i < vectorPerContact.size(); i++)
    {
        if ((id == vectorPerContact[i].getId() ))
        {
            return vectorPerContact[i];
        }
    }
}

OrganisationContact ContactRepository::getOrgObj(string id)
{
    for (unsigned long long i = 0; i < vectorOrgContact.size(); i++)
    {
        if ((id == vectorOrgContact[i].getId() ))
        {
            return vectorOrgContact[i];
        }
    }
}

void ContactRepository::updPerObj(string id, PersonalContact upd )
{
    for (unsigned long long i = 0; i < vectorPerContact.size(); i++)
    {
        if ((id == vectorPerContact[i].getId() ))
        {
            vectorPerContact[i].setFirstname(upd.getFirstname());
            vectorPerContact[i].setLastname(upd.getLastname());
            vectorPerContact[i].setCountry(upd.getCountry());
            vectorPerContact[i].setPhoneNumber(upd.getPhoneNumber());
            vectorPerContact[i].setCity(upd.getCity());
            vectorPerContact[i].setAddress(upd.getAddress());
            vectorPerContact[i].setComment(upd.getComment());
        }
    }
}

void ContactRepository::updOrgObj(string id, OrganisationContact upd)
{
    for (unsigned long long i = 0; i < vectorOrgContact.size(); i++)
    {
        if ((id == vectorOrgContact[i].getId() ))
        {
            vectorOrgContact[i].setNameOrg(upd.getNameOrg());
            vectorOrgContact[i].setPhoneNumber(upd.getPhoneNumber());
            vectorOrgContact[i].setCountry(upd.getCountry());
            vectorOrgContact[i].setCity(upd.getCity());
            vectorOrgContact[i].setAddress(upd.getAddress());
            vectorOrgContact[i].setComment(upd.getComment());
        }
    }
}

void ContactRepository::writeFilePerContact() {
    ofstream fout("PersonalContactDB.txt");
    for (unsigned long long i = 0; i < vectorPerContact.size(); i++) {
        fout << vectorPerContact[i].getId() << "/"
             << vectorPerContact[i].getFirstname() << "/"
             << vectorPerContact[i].getLastname() << "/"
             << vectorPerContact[i].getPhoneNumber() << "/"
             << vectorPerContact[i].getCountry() << "/"
             << vectorPerContact[i].getCity() << "/"
             << vectorPerContact[i].getAddress() << "/";
        if(i == (vectorPerContact.size()-1)) fout << this->vectorPerContact[i].getComment() << "/.";
        else fout << this->vectorPerContact[i].getComment() << "/";
    }
    fout.close();
}

void ContactRepository::writeFileOrgContact() {
    ofstream fout("OrganisationContactDB.txt");
    for (unsigned long long i = 0; i < vectorOrgContact.size(); i++) {
        fout << vectorOrgContact[i].getId() << "/"
             << vectorOrgContact[i].getNameOrg() << "/"
             << vectorOrgContact[i].getPhoneNumber() << "/"
             << vectorOrgContact[i].getCountry() << "/"
             << vectorOrgContact[i].getCity() << "/"
             << vectorOrgContact[i].getAddress() << "/";
        if(i == (vectorOrgContact.size()-1)) fout << this->vectorOrgContact[i].getComment() << "/.";
        else fout << this->vectorOrgContact[i].getComment() << "/";
    }
    fout.close();
}

void ContactRepository::readFileOrgContact() {
    vectorOrgContact.clear();

    OrganisationContact *a = new OrganisationContact();
    ifstream ins("OrganisationContactDB.txt");
    char stop;
        while (1) {
            ins.get(stop);
            if (stop == '.')
                break;
            else
                ins.seekg(-1, ins.cur);

            getline(ins, a->_id, '/');
            a->setFileId(a->_id);

            getline(ins, a->_nameOrg, '/');
            a->setNameOrg(a->_nameOrg);

            getline(ins, a->_phoneNumber, '/');
            a->setPhoneNumber(a->_phoneNumber);

            getline(ins, a->_country, '/');
            a->setCountry(a->_country);

            getline(ins, a->_city, '/');
            a->setCity(a->_city);

            getline(ins, a->_address, '/');
            a->setAddress(a->_address);

            getline(ins, a->_comment, '/');
            a->setComment(a->_comment);

            vectorOrgContact.push_back(*a);
        }
    ins.close();

}

void ContactRepository::readFilePerContact() {
    vectorPerContact.clear();

    PersonalContact *b = new PersonalContact();
    ifstream ins("PersonalContactDB.txt");
    char stop;
        while (1) {
            ins.get(stop);
            if (stop == '.')
                break;
            else
                ins.seekg(-1, ins.cur);

            getline(ins, b->_id, '/');
            b->setFileId(b->_id);

            getline(ins, b->_firstname, '/');
            b->setFirstname(b->_firstname);

            getline(ins, b->_lastname, '/');
            b->setLastname(b->_lastname);

            getline(ins, b->_phoneNumber, '/');
            b->setPhoneNumber(b->_phoneNumber);

            getline(ins, b->_country, '/');
            b->setCountry(b->_country);

            getline(ins, b->_city, '/');
            b->setCity(b->_city);

            getline(ins, b->_address, '/');
            b->setAddress(b->_address);

            getline(ins, b->_comment, '/');
            b->setComment(b->_comment);

            vectorPerContact.push_back(*b);
        }
    ins.close();

}

string ContactRepository::checkNumber(string number){
    string num = number;
    for(unsigned long long i=0; i < num.length();i++){
        if(!((num[i] >= '0' && num[i] <= '9') || num[0] == '+' || num[i] == ' '))
            return "";
    }
    if(num.length() > 20)
        return "";

    return num;
}
