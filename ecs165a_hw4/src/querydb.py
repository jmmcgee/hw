#!/usr/bin/python2
# -*- coding: utf-8 -*-

import psycopg2
import sys
import os
import csv

con = None
db = 'postgres'
host = '/home/' + os.environ['USER'] + '/postgres'
user = os.environ['USER']
srcRoot = 'src'

def connect():
    global con
    con = psycopg2.connect(database=db, host=host, user=user)
    cur = con.cursor()
    cur.execute('SELECT version()')
    ver = cur.fetchone()
    print ver
    cur.close()

def queryData():
    print "TODO: queryData(): Update to use thoughtful schema, load all tables"
    cur = con.cursor()

    # query Household values
    query = 'SELECT * FROM Household LIMIT 10'
    cur.execute(query);
    print 'Household'
    for values in cur.fetchall():
        print values

    # query Person values
    query = 'SELECT * FROM Person LIMIT 10'
    cur.execute(query);
    print 'Person'
    for values in cur.fetchall():
        print values

    # query Vehicle values
    query = 'SELECT * FROM Vehicle LIMIT 10'
    cur.execute(query);
    print 'Vehicle'
    for values in cur.fetchall():
        print values

    # query TravelDay values
    query = 'SELECT * FROM TravelDay LIMIT 10'
    cur.execute(query);
    print 'TravelDay'
    for values in cur.fetchall():
        print values

    cur.close()

def query3a():
    print "query3a(): percent of people that travel 5-100 miles a day in 5-mile increments" 
    cur = con.cursor()
    query = open(srcRoot + '/query3a.sql', 'r').read()

    for maxMiles in xrange(5,105,5):
        q = query % maxMiles
        cur.execute(q);
        for values in cur.fetchone():   
            print maxMiles, values

    cur.close()

def query3b():
    print "query3b(): avg fuel economy for trip grouped as 3a histogram" 
    cur = con.cursor()
    query = open(srcRoot + '/query3b.sql', 'r').read()

    for maxMiles in xrange(5,105,5):
        q = query.format(maxMiles)
        cur.execute(q);
        for values in cur.fetchall():
            print maxMiles, values

    cur.close()

def query3c():
    print "query3c(): percent of transportation CO2 attributed to vehicles" 
    cur = con.cursor()
    query = open(srcRoot + '/query3c.sql', 'r').read()
    totalNumHouseholds = 117538000

    q = query.format(totalNumHouseholds)
    cur.execute(q, totalNumHouseholds);
    for values in cur.fetchall():
        print values

    cur.close()

def query3d():
    print "query3d(): change in CO2 over months of survey w/ hybrids in 20,40,60 mile ranges" 
    cur = con.cursor()
    q_gas = open(srcRoot + '/query3d_gas.sql', 'r').read()
    q_hybridLess = open(srcRoot + '/query3d_hybridLesser.sql', 'r').read()
    q_hybridGreat = open(srcRoot + '/query3d_hybridGreater.sql', 'r').read()
    totalNumHouseholds = 117538000

    for evrange in [20,40,60]:
        cur.execute(q_gas.format(totalNumHouseholds, evrange));
        gas = cur.fetchall();
        cur.execute(q_hybridLess.format(totalNumHouseholds, evrange));
        hybridLess = cur.fetchall();
        cur.execute(q_hybridGreat.format(totalNumHouseholds, evrange));
        hybridGreat = cur.fetchall();

        keys = []
        gasCO2 = {}
        hybridCO2 = {}
        for row in gas:
            year = row[0]
            month = row[1]
            keys.append("%04d%02d" % (year, month))
        for row in gas:
            year = row[0]
            month = row[1]
            CO2 = row[2]
            gasCO2["%04d%02d" % (year, month)] = CO2
        for row in hybridLess:
            year = row[0]
            month = row[1]
            CO2 = row[2]
            hybridCO2["%04d%02d" % (year, month)] = CO2
        for row in hybridGreat:
            year = row[0]
            month = row[1]
            CO2 = row[2]
            hybridCO2["%04d%02d" % (year, month)] += CO2
    
        for key in keys:
            print evrange, key,"gas=%9d\thybrid=%9d\t->\tdiff=%+8d" % (gasCO2[key],hybridCO2[key],hybridCO2[key]-gasCO2[key])

    cur.close()

# Actually do stuff after all
try:
    connect()
    #queryData()
    query3a()
    query3b()
    query3c()
    query3d()

except psycopg2.DatabaseError, e:
    print 'Error %s' % e
    sys.exit(1)
finally:
    if con:
        con.close()

