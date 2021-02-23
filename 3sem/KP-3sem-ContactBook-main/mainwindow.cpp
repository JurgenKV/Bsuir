#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <QtWidgets/QApplication>
#include "QStandardItemModel"
#include "QStandardItem"
#include <QTableView>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <iomanip>
#include <fstream>

#include <contact.h>
#include <organisationcontact.h>
#include <personalcontact.h>
#include "contactrepository.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    repository = new ContactRepository;

    ui->tabWidget->setVisible(false);

    model = new QStandardItemModel( 0, 6 , this);

    ui->tableView->setModel(model);

    countContacts();

    model -> setHeaderData(0, Qt::Horizontal, "Имя");
    model -> setHeaderData(1, Qt::Horizontal, "Номер");
    model -> setHeaderData(2, Qt::Horizontal, "Страна");
    model -> setHeaderData(3, Qt::Horizontal, "Город");
    model -> setHeaderData(4, Qt::Horizontal, "Адрес");
    model -> setHeaderData(5, Qt::Horizontal, "Комментарий");
    ui->CitizradioButton->setChecked(1);
    ui->setTablePerradioButton->setChecked(1);

    ui->updateOrgpushButton->setVisible(false);
    ui->updatePerpushButton->setVisible(false);

    ifstream fileOrgContactDB("OrganisationContactDB.txt");
    ifstream filePerContactDB("PersonalContactDB.txt");

    if (!is_emptyFile(fileOrgContactDB))
    {
        repository->readFileOrgContact();
    }

    if (!is_emptyFile(filePerContactDB))
    {
        repository->readFilePerContact();
    }

    repository->writeFileOrgContact();
    repository->writeFilePerContact();

    on_setTablePerradioButton_pressed();

    countContacts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::is_emptyFile(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}

void MainWindow::on_pushButton_clicked()
{
    ui->tabWidget->setVisible(true);
    ui->createOrgpushButton_3->setVisible(true);
    ui->CreateContactCitizpushButton_2->setVisible(true);
    ui->updateOrgpushButton->setVisible(false);
    ui->updatePerpushButton->setVisible(false);
    contactClear();
}

void MainWindow::on_action_4_triggered()
{
    repository->writeFileOrgContact();
    repository->writeFilePerContact();
    QApplication::quit();
}

void MainWindow::on_exitMainpushButton_clicked()
{
    QMessageBox::StandardButton exit = QMessageBox::question(this,"Выход","Вы точно хотите выйти?", QMessageBox::Yes | QMessageBox::No);
    if(exit == QMessageBox::Yes)
        QApplication::exit();
}

void MainWindow::on_CreateContactCitizpushButton_2_clicked()
{
    string mainLineName = ui->nameCitizlineEdit_3->text().toStdString();
    string mainLineLastName = ui->lastnameCitizlineEdit_4->text().toStdString();
    string mainLineNumber = ui->numberCitizrlineEdit_5->text().toStdString();
    mainLineNumber = repository->checkNumber(mainLineNumber);

     if(repository->searchContactNumberOrg(mainLineNumber)== false && repository->searchContactNumberPer(mainLineNumber)== false){
        if (mainLineName != "" && mainLineLastName != "" && mainLineNumber != "") {
            PersonalContact *pContact = new PersonalContact();
            *pContact = BuildPerContact(pContact);
            repository->CreatePerContact(*pContact);
            repository->writeFilePerContact();

            ui->tabWidget->setVisible(false);

            contactClear();

            countContacts();

            on_setTablePerradioButton_pressed();
        }else
            QMessageBox::warning(this,"Нельзя создать контакт","Возможные причины: 1.Контакт с таким номером уже существует 2. Не все обязательные поля заполнены 3. Номер введен некорректно  ");
    }else
        QMessageBox::warning(this,"Нельзя создать контакт","Возможные причины: 1.Контакт с таким номером уже существует 2. Не все обязательные поля заполнены 3. Номер введен некорректно  ");
}

void MainWindow::on_ClearCitizpushButton_3_clicked()
{
    contactClear();
}

void MainWindow::on_createOrgpushButton_3_clicked()
{
    string mainLineName = ui->nameOrglineEdit_9->text().toStdString();
    string mainLineNumber = ui->numberOrglineEdit_10->text().toStdString();
    mainLineNumber = repository->checkNumber(mainLineNumber);
    repository->searchContactNumberOrg(mainLineNumber);
    if(repository->searchContactNumberOrg(mainLineNumber)== false && repository->searchContactNumberPer(mainLineNumber)== false){
    if (mainLineName != "" && mainLineNumber != ""){
        OrganisationContact *oContact = new OrganisationContact();
        if (mainLineName != "" && mainLineNumber != ""){
            *oContact = BuildOrgContact(oContact);
            repository->CreateOrgContact(*oContact);
            repository->writeFileOrgContact();

            ui->tabWidget->setVisible(false);

            contactClear();

            countContacts();

            on_setTableOrgradioButton_2_pressed();
        }
    }else{
        QMessageBox::warning(this,"Нельзя создать контакт","Возможные причины: 1.Контакт с таким номером уже существует 2. Не все обязательные поля заполнены 3. Номер введен некорректно  ");
    }
    }else
         QMessageBox::warning(this,"Нельзя создать контакт","Возможные причины: 1.Контакт с таким номером уже существует 2. Не все обязательные поля заполнены 3. Номер введен некорректно  ");
}
void MainWindow::contactClear(){

    ui->nameOrglineEdit_9->clear();
    ui->numberOrglineEdit_10->clear();
    ui->countryOrglineEdit_11->clear();
    ui->cityOrglineEdit_12->clear();
    ui->adresOrglineEdit_3->clear();
    ui->commentOrglineEdit_4->clear();
    ui->nameCitizlineEdit_3->clear();

    ui->lastnameCitizlineEdit_4->clear();
    ui->numberCitizrlineEdit_5->clear();
    ui->contryCitizlineEdit_6->clear();
    ui->cityCitizlineEdit_7->clear();
    ui->adresCitizlineEdit_3->clear();
    ui->commentCitizlineEdit_4->clear();
}

void MainWindow::on_clearOrgpushButton_2_clicked()
{
    contactClear();
}

void MainWindow::countContacts() // ок
{
    int a = repository->getVecSizeContact();
    QString s = QString::number(a);
    ui->countPersons->setText(s);
}

void MainWindow::on_delpushButton_clicked()
{

    if(ui->setTableOrgradioButton_2->isChecked())
    {
        if((ui->tableView->currentIndex().isValid())){
            int a = ui->tableView->currentIndex().row();
            string s = ui->tableView->model()->data(ui->tableView->model()->index(a,1)).toString().toStdString();
            int z = 0;
            ClearTable(1);
            repository->Delete(repository->searchContact(s),z);
            on_setTableOrgradioButton_2_pressed();
            repository->writeFileOrgContact();
            a=0;
        }
        else {
            QMessageBox::information(this,"Удаление контакта","Чтобы удалить контакт, нужно выбрать его в таблице");
        }
    }

    if(ui->setTablePerradioButton->isChecked())
    {
        if((ui->tableView->currentIndex().isValid())){
            int b = ui->tableView->currentIndex().row();
            string s = ui->tableView->model()->data(ui->tableView->model()->index(b,1)).toString().toStdString();
            int z = 1;
            ClearTable(1);
            repository->Delete(repository->searchContact(s),z);
            on_setTablePerradioButton_pressed();
            repository->writeFilePerContact();
            b=0;
        }else {
            QMessageBox::information(this,"Удаление контакта","Чтобы удалить контакт, нужно выбрать его в таблице");
        }
    }
    countContacts();
}

OrganisationContact MainWindow::BuildOrgContact(OrganisationContact *oContact)
{

    oContact->setNameOrg(ui->nameOrglineEdit_9->text().toStdString());
    oContact->setPhoneNumber(ui->numberOrglineEdit_10->text().toStdString());
    oContact->setCountry(ui->countryOrglineEdit_11->text().toStdString());
    oContact->setCity(ui->cityOrglineEdit_12->text().toStdString());
    oContact->setAddress(ui->adresOrglineEdit_3->text().toStdString());
    oContact->setComment(ui->commentOrglineEdit_4->text().toStdString());
    return *oContact;
}

PersonalContact MainWindow::BuildPerContact(PersonalContact *pContact)
{
    pContact->setFirstname(ui->nameCitizlineEdit_3->text().toStdString());
    pContact->setLastname(ui->lastnameCitizlineEdit_4->text().toStdString());
    pContact->setPhoneNumber(ui->numberCitizrlineEdit_5->text().toStdString());
    pContact->setCountry(ui->contryCitizlineEdit_6->text().toStdString());
    pContact->setCity(ui->cityCitizlineEdit_7->text().toStdString());
    pContact->setAddress(ui->adresCitizlineEdit_3->text().toStdString());
    pContact->setComment(ui->commentCitizlineEdit_4->text().toStdString());
    return *pContact;
}

void MainWindow::setColummDataPer(int lastRowIndex, PersonalContact *pContact){
    int col = 0;
    QModelIndex rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(pContact->getFullName()));
    col++;
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(pContact->getPhoneNumber()));
    col++;
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(pContact->getCountry()));
    col++;
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(pContact->getCity()));
    col++;
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(pContact->getAddress()));
    col++;
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(pContact->getComment()));
}

void MainWindow::setColummDataOrg(int lastRowIndex, OrganisationContact *oContact)
{
    int col = 0;
    QModelIndex rowColIndex = model->index(lastRowIndex,col);
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(oContact->getNameOrg()));
    col++;
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(oContact->getPhoneNumber()));
    col++;
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(oContact->getCountry()));
    col++;
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(oContact->getCity()));
    col++;
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(oContact->getAddress()));
    col++;
    rowColIndex = model->index(lastRowIndex,col);
    model->setData(rowColIndex, QString::fromStdString(oContact->getComment()));
}

void MainWindow::on_searchpushButton_clicked()
{
    string search = ui->lineEdit_search->text().toStdString();
    ui->lineEdit_search->clear();
    if(search != ""){
        if(ui->CitizradioButton->isChecked()){
            PersonalContact *pContact = new PersonalContact();

            if(repository->searchPerContactBool(search,0) == true){
                ClearTable(0);
                int j = 0;
                for (int i = 0; i < repository->getVecSizePerContact(); i++)
                {
                    if(repository->searchPerContactBool(search,i) == true){

                        i=repository->searchPerContactPosition(search, i);

                        *pContact = repository->searchPerContact(search, i);
                        model->insertRows(j,1);
                        setColummDataPer(j , pContact);
                        j++;
                    }
                }
            }else
                QMessageBox::information(this,"Поиск","Контакт не найдет");
            return;
        }
        if(ui->OrgradioButton->isChecked()){
            OrganisationContact *pContact = new OrganisationContact();

            if(repository->searchOrgContactBool(search,0) == true){
                ClearTable(0);
                int j = 0;
                for (int i = 0; i < repository->getVecSizeOrgContact(); i++)
                {
                    if(repository->searchOrgContactBool(search,i) == true){

                        i=repository->searchOrgContactPosition(search, i);

                        *pContact = repository->searchOrgContact(search, i);
                        model->insertRows(j,1);
                        setColummDataOrg(j , pContact);
                        j++;
                    }
                }
            }else
                QMessageBox::information(this,"Поиск","Контакт не найдет");
            return;
        }
    }else{
        if(ui->setTablePerradioButton->isChecked()){
            on_setTablePerradioButton_pressed();
        }
        if(ui->setTableOrgradioButton_2->isChecked()){
            on_setTableOrgradioButton_2_pressed();
        }
    }
}
void MainWindow::ClearTable(int c)
{
    if(repository->getVecSizeOrgContact() >= repository->getVecSizePerContact()) {
        for(int i = 0; i < repository->getVecSizeOrgContact()+ c; i++)
            model->removeRows(ui->tableView->rowAt(i) , 1);
    }else if(repository->getVecSizeOrgContact() < repository->getVecSizePerContact()) {
        for(int i = 0; i < repository->getVecSizePerContact() + c; i++)
            model->removeRows(ui->tableView->rowAt(i) , 1);
    }
}

void MainWindow::on_setTablePerradioButton_pressed()
{
    ui->CitizradioButton->setChecked(1);
    ui->OrgradioButton->setChecked(0);
    ClearTable(0);
    for(int k = 0; k  < repository->getVecSizePerContact() ; k++ ){
        PersonalContact *pContact = new PersonalContact();
        *pContact = repository->PerContactToTable(k);
        model->insertRows(k,1);
        setColummDataPer(k , pContact);
    }
}

void MainWindow::on_setTableOrgradioButton_2_pressed()
{
    ui->CitizradioButton->setChecked(0);
    ui->OrgradioButton->setChecked(1);
    ClearTable(0);
    for(int k = 0; k  < repository->getVecSizeOrgContact() ; k++ ){
        OrganisationContact *oContact = new OrganisationContact();
        *oContact = repository->OrgContactToTable(k);
        model->insertRows(k,1);
        setColummDataOrg(k, oContact);
    }
}

void MainWindow::on_updatePerpushButton_clicked()
{
    string mainLineName = ui->nameCitizlineEdit_3->text().toStdString();
    string mainLineLastName = ui->lastnameCitizlineEdit_4->text().toStdString();
    string mainLineNumber = ui->numberCitizrlineEdit_5->text().toStdString();

    mainLineNumber = repository->checkNumber(mainLineNumber);

    int a = ui->tableView->currentIndex().row();
    string s = ui->tableView->model()->data(ui->tableView->model()->index(a,1)).toString().toStdString();

    if((repository->searchContactNumberOrg(mainLineNumber)== false && repository->searchContactNumberPer(mainLineNumber)== false) || (s == mainLineNumber)){
         mainLineNumber = repository->checkNumber(mainLineNumber);
    if (mainLineName != "" && mainLineNumber != "" && mainLineNumber != "") {
        if((ui->tableView->currentIndex().isValid())){
            if(ui->setTablePerradioButton->isChecked()){
                int a = ui->tableView->currentIndex().row();
                string s = ui->tableView->model()->data(ui->tableView->model()->index(a,1)).toString().toStdString();
                PersonalContact *pContact = new PersonalContact();
                *pContact = repository->getPerObj(repository->searchContact(s));
                string id = pContact->getId();
                *pContact = BuildPerContact(pContact);
                repository->updPerObj(id, *pContact);
                ClearTable(0);
                on_setTablePerradioButton_pressed();
                repository->writeFilePerContact();
                contactClear();
            }else
                QMessageBox::warning(this,"Нельзя обновить контакт","Неправильно выбрана категория контакта");
        }
        on_clearOrgpushButton_2_clicked();
        ui->tabWidget->setVisible(false);
    }else
        QMessageBox::warning(this,"Нельзя обновить контакт","Возможные причины: 1.Контакт с таким номером уже существует 2. Не все обязательные поля заполнены 3. Номер введен некорректно  ");
}else
   QMessageBox::warning(this,"Нельзя обновить контакт","Возможные причины: 1.Контакт с таким номером уже существует 2. Не все обязательные поля заполнены 3. Номер введен некорректно  ");
}

void MainWindow::on_updateOrgpushButton_clicked()
{
    string mainLineName = ui->nameOrglineEdit_9->text().toStdString();
    string mainLineNumber = ui->numberOrglineEdit_10->text().toStdString();
    int a = ui->tableView->currentIndex().row();
    string s = ui->tableView->model()->data(ui->tableView->model()->index(a,1)).toString().toStdString();

  if((repository->searchContactNumberOrg(mainLineNumber)== false && repository->searchContactNumberPer(mainLineNumber)== false) || (s == mainLineNumber)){
     mainLineNumber= repository->checkNumber(mainLineNumber);
    if (mainLineName != ""  && mainLineNumber != "") {
        if((ui->tableView->currentIndex().isValid())){
            if(ui->setTableOrgradioButton_2->isChecked()){
                int a = ui->tableView->currentIndex().row();
                string s = ui->tableView->model()->data(ui->tableView->model()->index(a,1)).toString().toStdString();

                OrganisationContact *oContact = new OrganisationContact();
                *oContact = repository->getOrgObj(repository->searchContact(s));
                string id = oContact->getId();
                *oContact = BuildOrgContact(oContact);
                repository->updOrgObj(id, *oContact);
                ClearTable(0);
                on_setTableOrgradioButton_2_pressed();
                repository->writeFileOrgContact();
                contactClear();

            }else {
                QMessageBox::information(this,"Обновление контакта","Неправильно выбрана категория контакта");
            }
        }
        on_clearOrgpushButton_2_clicked();
        ui->tabWidget->setVisible(false);
    }
    else{
        QMessageBox::warning(this,"Нельзя обновить контакт","Возможные причины: 1.Контакт с таким номером уже существует 2. Не все обязательные поля заполнены 3. Номер введен некорректно  ");
  }
}else
      QMessageBox::warning(this,"Нельзя обновить контакт","Возможные причины: 1.Контакт с таким номером уже существует 2. Не все обязательные поля заполнены 3. Номер введен некорректно  ");
}
void MainWindow::on_UpdateContactpushButton_clicked()
{
    on_clearOrgpushButton_2_clicked();

    if(repository->getVecSizeContact()!= 0) {

        ui->tabWidget->setVisible(true);

        ui->updateOrgpushButton->setVisible(false);
        ui->updatePerpushButton->setVisible(false);

        if(ui->setTableOrgradioButton_2->isChecked()){
            ui->updateOrgpushButton->setVisible(true);
            ui->updatePerpushButton->setVisible(false);
        }
        if(ui->setTablePerradioButton->isChecked()){
            ui->updateOrgpushButton->setVisible(false);
            ui->updatePerpushButton->setVisible(true);
        }

        ui->createOrgpushButton_3->setVisible(false);
        ui->CreateContactCitizpushButton_2->setVisible(false);
    if(ui->setTableOrgradioButton_2->isChecked()){
        if((ui->tableView->currentIndex().isValid())){
            if(repository->getVecSizeOrgContact()!= 0){
                if(ui->setTableOrgradioButton_2->isChecked()){
                    int a = ui->tableView->currentIndex().row();
                    string s = ui->tableView->model()->data(ui->tableView->model()->index(a,1)).toString().toStdString();
                    OrganisationContact *oContact = new OrganisationContact();
                    *oContact = repository->getOrgObj(repository->searchContact(s));

                    ui->nameOrglineEdit_9->setText(QString::fromStdString(oContact->getNameOrg()));
                    ui->numberOrglineEdit_10->setText(QString::fromStdString(oContact->getPhoneNumber()));
                    ui->countryOrglineEdit_11->setText(QString::fromStdString(oContact->getCountry()));
                    ui->cityOrglineEdit_12->setText(QString::fromStdString(oContact->getCity()));
                    ui->adresOrglineEdit_3->setText(QString::fromStdString(oContact->getAddress()));
                    ui->commentOrglineEdit_4->setText(QString::fromStdString(oContact->getComment()));

                }
            }
        }
    }
    if(ui->setTablePerradioButton->isChecked()){
        if((ui->tableView->currentIndex().isValid())){
            if(repository->getVecSizePerContact()!= 0){
                if(ui->setTablePerradioButton->isChecked()){
                    int a = ui->tableView->currentIndex().row();
                    string s = ui->tableView->model()->data(ui->tableView->model()->index(a,1)).toString().toStdString();
                    PersonalContact *pContact = new PersonalContact();
                    *pContact = repository->getPerObj(repository->searchContact(s));

                    ui->nameCitizlineEdit_3->setText(QString::fromStdString(pContact->getFirstname()));
                    ui->lastnameCitizlineEdit_4->setText(QString::fromStdString(pContact->getLastname()));
                    ui->numberCitizrlineEdit_5->setText(QString::fromStdString(pContact->getPhoneNumber()));
                    ui->contryCitizlineEdit_6->setText(QString::fromStdString(pContact->getCountry()));
                    ui->cityCitizlineEdit_7->setText(QString::fromStdString(pContact->getCity()));
                    ui->adresCitizlineEdit_3->setText(QString::fromStdString(pContact->getAddress()));
                    ui->commentCitizlineEdit_4->setText(QString::fromStdString(pContact->getComment()));
                }
            }
        }
    }
    }else
        QMessageBox::critical(this,"Ошибка", "Чтобы обновить контакт, нужно его сначала создать ");
}

void MainWindow::on_CitizradioButton_pressed()
{
    ui->setTableOrgradioButton_2->setChecked(0);
    ui->setTablePerradioButton->setChecked(1);
    on_setTablePerradioButton_pressed();
}

void MainWindow::on_OrgradioButton_clicked()
{
    ui->setTableOrgradioButton_2->setChecked(1);
    ui->setTablePerradioButton->setChecked(0);
    on_setTableOrgradioButton_2_pressed();
}

void MainWindow::on_closeTabWpushButton_2_clicked()
{
    ui->tabWidget->setVisible(false);
    contactClear();
}

void MainWindow::on_closeTabWPerpushButton_2_clicked()
{
    ui->tabWidget->setVisible(false);
    contactClear();
}

