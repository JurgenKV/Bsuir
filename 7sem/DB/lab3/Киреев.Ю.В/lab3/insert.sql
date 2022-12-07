INSERT INTO public."EMPLOYEE"("Login", "Password", "Age", "FullName", "HALL_ID")
	VALUES ('Login4', '123', '23', 'Роман Запольский', '1'),
	VALUES ('Login3', '123', '22', 'Валерий Жмах', '1'),
	VALUES ('Login2', '123', '29', 'Александр Шляпик', '1'),
	VALUES ('Login5', '123', '18', 'Евгений Шпак', '1'),
	VALUES ('Login6', '123', '40', 'Николай Ликшик', '1');

INSERT INTO public."EMPLOYEE-POSITION"("EMPLOYEE_ID", "POSITION_ID")
	VALUES (5, 4),
	VALUES (4, 2),
	VALUES (3, 1),
	VALUES (2, 3),
	VALUES (6, 5);

INSERT INTO public."FILM"("Name", "Description", "SESSION_ID")
	VALUES ('Шрек 3', 'Семейный', 3),
	VALUES ('Шрек 2', 'Семейный', 2),
	VALUES ('Шрек 1', 'Семейный', 1),
	VALUES ('Брат', 'Классика', 4),
	VALUES ('Брат 2', 'Классика', 5);

INSERT INTO public."HALL"("Name", "SESSION_ID")
	VALUES ('Зал 1', 1),
	VALUES ('Зал 2', 2),
	VALUES ('Зал 3', 3),
	VALUES ('Зал 4', 4),
	VALUES ('Зал 5', 5);

INSERT INTO public."HALLROW"("Number", "Count", "HALL_ID", "OCCUPANCY_ID")
	VALUES ('1', '10', 1, 1),
	VALUES ('2', '10', 1, 2),
	VALUES ('3', '10', 1, 3),
	VALUES ('4', '10', 1, 4),
	VALUES ('5', '10', 1, 5),
	VALUES ('6', '10', 1, 6),
	VALUES ('7', '10', 1, 7),
	VALUES ('8', '10', 1, 8),
	VALUES ('9', '10', 1, 9),
	VALUES ('10', '10', 1, 10),
	VALUES ('11', '10', 1, 11),
	VALUES ('12', '10', 1, 12);

INSERT INTO public."OCCUPANCY"("Place", "Row", "SESSION_ID")
	VALUES (1, 1, 1),
	VALUES (2, 2, 1),
	VALUES (3, 3, 1),
	VALUES (4, 4, 1),
	VALUES (5, 5, 1),
	VALUES (6, 6, 1),
	VALUES (7, 7, 1),
	VALUES (8, 8, 1),
	VALUES (9, 9, 1),
	VALUES (10, 10, 1),
	VALUES (11, 11, 1),
	VALUES (12, 12, 1);

INSERT INTO public."POSITION"("Name", "Salary")
	VALUES ('Кассир', 1000),
	VALUES ('Менеджер', 1500),
	VALUES ('Уборщик', 900),
	VALUES ('Инженер', 2100),
	VALUES ('Директор', 3000);

INSERT INTO public."SESSION"("Time", "AmountPlaces")
	VALUES ('15:00:00+03:00', 120),
	VALUES ('13:00:00+03:00', 120),
	VALUES ('10:30:00+03:00', 120),
	VALUES ('18:00:00+03:00', 120),
	VALUES ('21:00:00+03:00', 120);

INSERT INTO public."TICKET"("Place", "Row", "Price", "EMPLOYEE_ID", "SESSION_ID", "HALLROW_ID")
	VALUES ('30', '3', 5, 3, 3, 3),
	VALUES ('20', '2', 4, 3, 2, 2),
	VALUES ('22', '2', 5, 3, 2, 2),
	VALUES ('40', '4', 5, 3, 4, 4),
	VALUES ('50', '5', 4, 3, 5, 5);