DROP TABLE IF EXISTS Household, Person, Vehicle, TravelDay;

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
    PRIMARY KEY(houseid, vehid)
);

CREATE TABLE TravelDay (
    houseid INTEGER, -- 1
    personid INTEGER, -- 2
    tdcaseid BIGINT, -- 20
    PRIMARY KEY(houseid, personid, tdcaseid)
);

