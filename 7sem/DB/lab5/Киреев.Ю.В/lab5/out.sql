********* ЗАПРОС *********
SELECT SUM ("Count")
	FROM public."HALLROW"
	JOIN public."TICKET" tic on tic."ID" = public."HALLROW"."ID"
	where "Price" = 5;
**************************
 sum
-----
  30

********* ЗАПРОС *********
SELECT MIN("Count")
	FROM public."HALLROW"
	FULL JOIN public."OCCUPANCY" occ on occ."ID" = "HALLROW"."OCCUPANCY_ID";
**************************
 min
-----
  10

********* ЗАПРОС *********
SELECT Count("EMPLOYEE"."ID")
	FROM public."EMPLOYEE"
	FULL join public."EMPLOYEE-POSITION" ep on "EMPLOYEE"."ID" = ep."EMPLOYEE_ID"
	where ep."POSITION_ID" = (select "ID" from "POSITION" where "Salary" = 3000);
**************************
 count
-------
     1
********* ЗАПРОС *********
SELECT "Name", "Salary"
	FROM public."POSITION"
	where "Salary" = (select MAX("Salary") FROM public."POSITION");
**************************

   Name   | Salary
----------+--------
 Директор |   3000

********* ЗАПРОС *********
SELECT "FullName"
	FROM public."EMPLOYEE"
	JOIN public."HALL" hall on hall."ID" = public."EMPLOYEE"."ID" 
	group by "EMPLOYEE"."FullName";
**************************

     FullName
------------------
 Евгений Шпак
 Роман Запольский
 Валерий Жмах
 Александр Шляпик

********* ЗАПРОС *********
SELECT "EMPLOYEE"."FullName", "HALL"."Name"
	FROM public."EMPLOYEE",  public."HALL"
	where "HALL"."ID" = "EMPLOYEE"."HALL_ID" and "HALL"."Name" IN ('Зал 1','Зал 3') order by "HALL"."Name";
**************************

     FullName     | Name
------------------+-------
 Роман Запольский | Зал 1
 Валерий Жмах     | Зал 1
 Александр Шляпик | Зал 1
 Евгений Шпак     | Зал 3




