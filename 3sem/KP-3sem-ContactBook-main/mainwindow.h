#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <contact.h>
#include <QStandardItemModel>
#include <contactrepository.h>
#include <organisationcontact.h>
#include <personalcontact.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_action_4_triggered();

    void on_exitMainpushButton_clicked();

    void on_CreateContactCitizpushButton_2_clicked();

    void on_ClearCitizpushButton_3_clicked();

    void on_createOrgpushButton_3_clicked();

    void on_clearOrgpushButton_2_clicked();

    void countContacts();

    void contactClear();

    void on_delpushButton_clicked();

    OrganisationContact BuildOrgContact(OrganisationContact *oContact);

    PersonalContact BuildPerContact(PersonalContact *pContact);

    void setColummDataPer(int lastRowIndex, PersonalContact *pContact);

    void setColummDataOrg(int lastRowIndex, OrganisationContact *oContact);

    void on_searchpushButton_clicked();

    void ClearTable(int c);

    void on_setTablePerradioButton_pressed();

    void on_setTableOrgradioButton_2_pressed();

    void on_updatePerpushButton_clicked();

    void on_updateOrgpushButton_clicked();

    void on_UpdateContactpushButton_clicked();

    bool is_emptyFile(std::ifstream& pFile);

    void on_CitizradioButton_pressed();

    void on_OrgradioButton_clicked();

    void on_closeTabWpushButton_2_clicked();

    void on_closeTabWPerpushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QStandardItemModel *csvModel;
    ContactRepository *repository;
};
#endif // MAINWINDOW_H
