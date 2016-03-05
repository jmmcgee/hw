#!/usr/bin/python2
# -*- coding: utf-8 -*-

import psycopg2
import sys
import os
import csv

con = None
db = 'postgres'
host = '/home/' + os.environ['USER'] + '/postgres'
dataRoot = '../data'

dropAll = """
DROP TABLE IF EXISTS Household, Person, Vehicle, TravelDay
"""

householdTable = """
CREATE TABLE Household (
    houseid INTEGER, -- 1
    PRIMARY KEY(houseid)
);
"""

personTable = """
CREATE TABLE Person (
    houseid INTEGER, -- 1
    personid INTEGER, -- 2
    PRIMARY KEY(houseid, personid)
);
"""

vehicleTable = """
CREATE TABLE Vehicle (
    houseid INTEGER, -- 1
    vehid INTEGER, -- 3
    PRIMARY KEY(houseid, vehid)
);
"""

travelDayTable = """
CREATE TABLE TravelDay (
    houseid INTEGER, -- 1
    personid INTEGER, -- 2
    tdcaseid INTEGER, -- 20
    PRIMARY KEY(houseid, personid, tdcaseid)
);
"""
def connect():
    global con
    con = psycopg2.connect(database=db, host=host)
    cur = con.cursor()
    cur.execute('SELECT version()')
    ver = cur.fetchone()
    print ver
    cur.close()

def loadSchema():
    print "TODO: implement loadSchema()"
    cur = con.cursor()
    cur.execute(dropAll)
    cur.execute(householdTable)
    cur.execute(personTable)
    cur.execute(vehicleTable)
    cur.execute(travelDayTable)
    cur.close()

def loadData():
    cur = con.cursor()

    # define query for inserting into Person
    query = 'INSERT INTO Person(houseid, personid) VALUES (%s , %s);'

    i = 0
    with open(dataRoot + '/Person.head.csv', 'r') as f:
        # read csv, throw away header
        reader = csv.reader(f)
        row = next(reader)
        for row in reader:
            data = (int(row[0]), int(row[1]))
            print i, data
            cur.execute(query, data)
            i = i + 1

    print "TODO: implement loadData()"


# Actually do stuff after all
try:
    connect()
    loadSchema()
    loadData()
except psycopg2.DatabaseError, e:
    print 'Error %s' % e
    sys.exit(1)
finally:
    if con:
        con.close()

