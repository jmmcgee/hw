DROP TABLE IF EXISTS Household, Person, Vehicle, TravelDay;
DROP TABLE IF EXISTS TransportationCO2, ElectricityCO2, Electricity;
DROP TABLE IF EXISTS DaysInMonth;

--- Auxillary

CREATE TABLE DaysInMonth (
    month INTEGER PRIMARY KEY,
    days INTEGER
);

---

CREATE TABLE Household (
    houseid INTEGER, -- 1

    year INTEGER, -- 30 (YYYY__)
    month INTEGER, -- 30 (____MM)
    PRIMARY KEY(houseid)
);

CREATE TABLE Person (
    houseid INTEGER, -- 1
    personid INTEGER, -- 2

    PRIMARY KEY(houseid, personid)
);

CREATE TABLE Vehicle (
    houseid INTEGER, -- 1
    vehid INTEGER, -- 3

    epatmpg DOUBLE PRECISION, -- 56, unadjusted 55/45 combined fuel economy

    PRIMARY KEY(houseid, vehid)
);

CREATE TABLE TravelDay (
    houseid INTEGER, -- 1
    personid INTEGER, -- 2
    tdtrpnum INTEGER, -- 92

    year INTEGER, -- 94 (YYYY__)
    month INTEGER, -- 94 (____MM)
    tdcaseid BIGINT, -- 20
    vehid INTEGER, -- 84
    trpmiles DOUBLE PRECISION, -- 95,  miles per trip (per day)

    PRIMARY KEY(houseid, personid, tdtrpnum)
);

CREATE TABLE TransportationCO2 (
    msn CHAR(7),
    year INTEGER,
    month INTEGER,
    value DOUBLE PRECISION,
    description TEXT,
    Unit TEXT,

    PRIMARY KEY(msn,year,month)
);

CREATE TABLE ElectricityCO2 (
    msn CHAR(7),
    year INTEGER,
    month INTEGER,
    value DOUBLE PRECISION,
    description TEXT,
    Unit TEXT,

    PRIMARY KEY(msn,year,month)
);

CREATE TABLE Electricity (
    msn CHAR(7),
    year INTEGER,
    month INTEGER,
    value DOUBLE PRECISION,
    description TEXT,
    Unit TEXT,

    PRIMARY KEY(msn,year,month)
);
