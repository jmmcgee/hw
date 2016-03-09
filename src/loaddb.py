#!/usr/bin/python2
# -*- coding: utf-8 -*-

import psycopg2
import sys
import os
import csv

DEBUG = True

con = None
db = 'postgres'
host = '/home/' + os.environ['USER'] + '/postgres'

dataRoot = 'data' if DEBUG == True else '/home/cjnitta/ecs165a'
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
    query = open(srcRoot + '/schema.sql','r').read()
    cur.execute(query);
    cur.close()

def loadData():
    print "TODO: loadData(): Update to use thoughtful schema, load all tables"
    cur = con.cursor()

    # load Household values
    columns = ["houseid"]
    query = 'INSERT INTO Household({0}) VALUES ({1});'
    query = query.format(", ".join(columns), ", ".join(["%s"] * len(columns)) )
    with open(dataRoot + '/HHV2PUB.CSV', 'r') as f:
        # read csv, throw away header
        reader = csv.reader(f)
        row = next(reader)
        for row in reader:
            houseid = int(row[0])

            values = (houseid,)
            cur.execute(query, values)
    con.commit()

    # load Person values
    columns = ["houseid","personid"]
    query = 'INSERT INTO Person({0}) VALUES ({1});'
    query = query.format(", ".join(columns), ", ".join(["%s"] * len(columns)) )
    with open(dataRoot + '/PERV2PUB.CSV', 'r') as f:
        # read csv, throw away header
        reader = csv.reader(f)
        row = next(reader)
        i = 0
        for row in reader:
            houseid = int(row[0])
            personid = int(row[1])

            values = (houseid, personid)
            cur.execute(query, values)
    con.commit()

    # load Vehicle values
    columns = ["houseid","vehid","epatmpg"]
    query = 'INSERT INTO Vehicle({0}) VALUES ({1});'
    query = query.format(", ".join(columns), ", ".join(["%s"] * len(columns)) )
    with open(dataRoot + '/VEHV2PUB.CSV', 'r') as f:
        # read csv, throw away header
        reader = csv.reader(f)
        row = next(reader)
        for row in reader:
            houseid = int(row[0])
            vehid = int(row[2])
            epatmpg = float(row[55])

            values = (houseid, vehid, epatmpg)
            cur.execute(query, values)
    con.commit()

    # load TravelDay values
    columns = ["houseid","personid","tdcaseid","tdtrpnum","vehid","trpmiles"]
    query = 'INSERT INTO TravelDay({0}) VALUES ({1});'
    query = query.format(", ".join(columns), ", ".join(["%s"] * len(columns)) )
    with open(dataRoot + '/DAYV2PUB.CSV', 'r') as f:
        # read csv, throw away header
        reader = csv.reader(f)
        row = next(reader)
        for row in reader:
            houseid = int(row[0])
            personid = int(row[1])
            tdtrpnum = int(row[91])
            tdcaseid = long(row[19])
            vehid = int(row[83])
            trpmiles = float(row[94])

            values = (houseid, personid, tdcaseid, tdtrpnum, vehid, trpmiles)
            cur.execute(query, values)
    con.commit()

    # load TransportationCO2 values
    columns = ["msn","year","month","value","description","unit"]
    query = 'INSERT INTO TransportationCO2({0}) VALUES ({1});'
    query = query.format(", ".join(columns), ", ".join(["%s"] * len(columns)) )
    with open(dataRoot + '/EIA_CO2_Transportation_2015.csv', 'r') as f:
        # read csv, throw away header
        reader = csv.reader(f)
        row = next(reader)
        for row in reader:
            msn = row[0]
            year = int(row[1][0:4])
            month = int(row[1][5:7])
            value = float(row[2])
            descrption = row[4]
            unit = row[5]

            values = (msn, year, month, value, descrption, unit)
            print msn, year, month
            cur.execute(query, values)
    con.commit()

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

