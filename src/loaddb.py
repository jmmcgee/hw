#!/usr/bin/python2
# -*- coding: utf-8 -*-

import psycopg2
import sys
import os
import csv

con = None
db = 'postgres'
host = '/home/' + os.environ['USER'] + '/postgres'
dataRoot = 'data'
srcRoot = 'src'

def connect():
    global con
    con = psycopg2.connect(database=db, host=host)
    cur = con.cursor()
    cur.execute('SELECT version()')
    ver = cur.fetchone()
    print ver
    cur.close()

def loadSchema():
    print "TODO: loadSchema(): create usefuel database schema"
    cur = con.cursor()
    query = open('src/schema.sql','r').read()
    cur.execute(query);
    cur.close()

def loadData():
    print "TODO: loadData(): Update to use thoughtful schema, load all tables"
    cur = con.cursor()

    # load Household values
    query = 'INSERT INTO Household(houseid) VALUES (%s);'
    with open(dataRoot + '/Household.head.csv', 'r') as f:
        # read csv, throw away header
        reader = csv.reader(f)
        row = next(reader)
        for row in reader:
            values = (int(row[0]),)
            cur.execute(query, values)

    # load Person values
    query = 'INSERT INTO Person(houseid, personid) VALUES (%s , %s);'
    with open(dataRoot + '/Person.head.csv', 'r') as f:
        # read csv, throw away header
        reader = csv.reader(f)
        row = next(reader)
        i = 0
        for row in reader:
            values = (int(row[0]), int(row[1]))
            cur.execute(query, values)

    # load Vehicle values
    query = 'INSERT INTO Vehicle(houseid, vehid) VALUES (%s , %s);'
    with open(dataRoot + '/Vehicle.head.csv', 'r') as f:
        # read csv, throw away header
        reader = csv.reader(f)
        row = next(reader)
        for row in reader:
            values = (int(row[0]), int(row[2]))
            cur.execute(query, values)

    # load TravelDay values
    query = 'INSERT INTO TravelDay(houseid, personid, tdcaseid) VALUES (%s , %s, %s);'
    with open(dataRoot + '/TravelDay.head.csv', 'r') as f:
        # read csv, throw away header
        reader = csv.reader(f)
        row = next(reader)
        for row in reader:
            values = (int(row[0]), int(row[1]), int(row[19]))
            cur.execute(query, values)
    cur.close()


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

