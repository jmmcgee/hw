DROP TABLE IF EXISTS Household, Person, Vehicle, TravelDay;
DROP TABLE IF EXISTS TransportationCO2;

CREATE TABLE Household (
    houseid INTEGER, -- 1

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

