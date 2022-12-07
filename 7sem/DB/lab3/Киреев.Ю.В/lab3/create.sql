CREATE TABLE IF NOT EXISTS public."EMPLOYEE"
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 2 MINVALUE 2 MAXVALUE 2147483647 ),
    "Login" character varying(255) COLLATE pg_catalog."default" NOT NULL,
    "Password" character varying(255) COLLATE pg_catalog."default" NOT NULL,
    "Age" character varying(255) COLLATE pg_catalog."default",
    "FullName" character varying(255) COLLATE pg_catalog."default",
    "HALL_ID" integer,
    CONSTRAINT "EMPLOYEE_pkey" PRIMARY KEY ("ID"),
    CONSTRAINT "EMPLOYEE_HALL_ID_fkey" FOREIGN KEY ("HALL_ID")
        REFERENCES public."HALL" ("ID") 
)

CREATE TABLE IF NOT EXISTS public."EMPLOYEE-POSITION"
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 ),
    "EMPLOYEE_ID" integer,
    "POSITION_ID" integer,
    CONSTRAINT "EMPLOYEE-POSITION_pkey" PRIMARY KEY ("ID"),
    CONSTRAINT "EMPLOYEE-POSITION_EMPLOYEE_ID_fkey" FOREIGN KEY ("EMPLOYEE_ID")
        REFERENCES public."EMPLOYEE" ("ID") 
    CONSTRAINT "EMPLOYEE-POSITION_POSITION_ID_fkey" FOREIGN KEY ("POSITION_ID")
        REFERENCES public."POSITION" ("ID") 
)

CREATE TABLE IF NOT EXISTS public."FILM"
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 ),
    "Name" character varying(255) COLLATE pg_catalog."default",
    "Description" text COLLATE pg_catalog."default",
    "SESSION_ID" integer,
    CONSTRAINT "FILM_pkey" PRIMARY KEY ("ID"),
    CONSTRAINT "FILM_SESSION_ID_fkey" FOREIGN KEY ("SESSION_ID")
        REFERENCES public."SESSION" ("ID") 
)

CREATE TABLE IF NOT EXISTS public."HALL"
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 ),
    "Name" character varying(255) COLLATE pg_catalog."default",
    "SESSION_ID" integer,
    CONSTRAINT "HALL_pkey" PRIMARY KEY ("ID"),
    CONSTRAINT "HALL_SESSION_ID_fkey" FOREIGN KEY ("SESSION_ID")
        REFERENCES public."SESSION" ("ID")
)

CREATE TABLE IF NOT EXISTS public."HALLROW"
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 ),
    "Number" character varying(255) COLLATE pg_catalog."default",
    "Count" integer,
    "HALL_ID" integer,
    "OCCUPANCY_ID" integer,
    CONSTRAINT "HALLROW_pkey" PRIMARY KEY ("ID"),
    CONSTRAINT "HALLROW_HALL_ID_fkey" FOREIGN KEY ("HALL_ID")
        REFERENCES public."HALL" ("ID") ,
    CONSTRAINT "HALLROW_OCCUPANCY_ID_fkey" FOREIGN KEY ("OCCUPANCY_ID")
        REFERENCES public."OCCUPANCY" ("ID") 
)

CREATE TABLE IF NOT EXISTS public."OCCUPANCY"
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 ),
    "Place" character varying(255) COLLATE pg_catalog."default",
    "Row" character varying(255) COLLATE pg_catalog."default",
    CONSTRAINT "OCCUPANCY_pkey" PRIMARY KEY ("ID")
    CONSTRAINT "OCCUPANCY_SESSION_ID_fkey" FOREIGN KEY ("OCCUPANCY_ID")
        REFERENCES public."SESSION" ("ID") 
)

CREATE TABLE IF NOT EXISTS public."POSITION"
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 ),
    "Name" character varying(255) COLLATE pg_catalog."default",
    "Salary" bigint,
    CONSTRAINT "POSITION_pkey" PRIMARY KEY ("ID")
)

CREATE TABLE IF NOT EXISTS public."SESSION"
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 ),
    "Time" time with time zone,
    "AmountPlaces" integer,
    "OCCUPANCY_ID" integer,
    CONSTRAINT "SESSION_pkey" PRIMARY KEY ("ID")
)


CREATE TABLE IF NOT EXISTS public."TICKET"
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 CACHE 1 ),
    "Place" character varying(255) COLLATE pg_catalog."default",
    "Row" character varying(255) COLLATE pg_catalog."default",
    "Price" bigint,
    "EMPLOYEE_ID" integer,
    "SESSION_ID" integer,
    "HALLROW_ID" integer,
    CONSTRAINT "TICKET_pkey" PRIMARY KEY ("ID"),
    CONSTRAINT "TICKET_EMPLOYEE_ID_fkey" FOREIGN KEY ("EMPLOYEE_ID")
        REFERENCES public."EMPLOYEE" ("ID"),
    CONSTRAINT "TICKET_HALLROW_ID_fkey" FOREIGN KEY ("HALLROW_ID")
        REFERENCES public."HALLROW" ("ID"),
    CONSTRAINT "TICKET_SESSION_ID_fkey" FOREIGN KEY ("SESSION_ID")
        REFERENCES public."SESSION" ("ID") 
)