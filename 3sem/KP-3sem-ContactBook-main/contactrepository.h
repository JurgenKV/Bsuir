#ifndef CONTACTREPOSITORY_H
#define CONTACTREPOSITORY_H

#include <contact.h>
#include <iostream>
#include <clocale>
#include <cstring>
#include <vector>
#include <personalcontact.h>
#include <organisationcontact.h>

using namespace std;

class ContactRepository
{
    private:
         vector <OrganisationContact> vectorOrgContact;

         vector <PersonalContact> vectorPerContact;

         vector <Contact> vector;
    public:
       void CreateContact(Contact a);

       void CreateOrgContact(OrganisationContact a);

       void CreatePerContact(PersonalContact a);

        void Delete(string id, int categoty);

       int getVecSizeContact();

        int getVecSizePerContact();

        int getVecSizeOrgContact();

       OrganisationContact searchOrgContact(string search, unsigned long long i);

       bool searchOrgContactBool(string search, unsigned long long i);

        unsigned long long searchOrgContactPosition(string search, unsigned long long i);

      PersonalContact searchPerContact(string search, unsigned long long i);

        bool searchPerContactBool(string search, unsigned long long i);

        unsigned long long searchPerContactPosition(string search, unsigned long long k);

        string searchContact(string search);

        string searchContactToUpdate(string search);

        string numVecId(unsigned long long a);

        bool idCheck(string id);

        PersonalContact PerContactToTable(int i);

        OrganisationContact OrgContactToTable(int i);

        PersonalContact getPerObj(string id);

        OrganisationContact getOrgObj(string id);

        void updPerObj(string id, PersonalContact upd );

        void updOrgObj(string id, OrganisationContact upd );

        void writeFilePerContact();

        void writeFileOrgContact();

        void readFileOrgContact();

        void readFilePerContact();

        bool searchContactNumberOrg(string search);

        bool searchContactNumberPer(string search);

        string checkNumber(string number);

  friend class Contact;
  friend class OrganisationContact;
  friend class PersonalContact;
};

#endif // CONTACTREPOSITORY_H
