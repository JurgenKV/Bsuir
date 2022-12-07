SELECT "Login", "Age", "FullName" FROM public."EMPLOYEE" WHERE CAST("Age" as Integer) < 30;

SELECT "Name", "Salary" FROM public."POSITION" WHERE "Salary" BETWEEN 2000 AND 3500;

SELECT "Place", "Row", "Price" FROM public."TICKET" WHERE "Price" = 4;

SELECT "Name" FROM public."HALL" ORDER BY "Name" DESC;

SELECT "Time", "AmountPlaces" FROM public."SESSION" ORDER BY "Time" ASC;

SELECT "Name", "Description" FROM public."FILM" WHERE "Description" IN ('Классика');

SELECT "Number", "Count" FROM public."HALLROW" WHERE "Count" = 10;

SELECT "Place", "Row" FROM public."OCCUPANCY";

SELECT "Login", "Age", "FullName" FROM public."EMPLOYEE" WHERE ("FullName" like '%ах' OR "FullName" like '%ик');


