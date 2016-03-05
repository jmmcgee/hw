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
dataRoot = 'data'
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
    query = 'SELECT * FROM Household LIMIT 3'
    cur.execute(query);
    print 'Household'
    for values in cur.fetchall():
        print values

    # query Person values
    query = 'SELECT * FROM Person LIMIT 3'
    cur.execute(query);
    print 'Person'
    for values in cur.fetchall():
        print values

    # query Vehicle values
    query = 'SELECT * FROM Vehicle LIMIT 3'
    cur.execute(query);
    print 'Vehicle'
    for values in cur.fetchall():
        print values

    # query TravelDay values
    query = 'SELECT * FROM TravelDay LIMIT 3'
    cur.execute(query);
    print 'TravelDay'
    for values in cur.fetchall():
        print values

def query3a():
    print "TODO: query3a(): percent of people that travel 5-100 miles a day in 5-mile increments" 

def query3b():
    print "TODO: query3b(): avg fuel economy for trip grouped as 3a histogram" 

def query3c():
    print "TODO: query3c(): percent of transportation CO2 attributed to vehicles" 

def query3d():
    print "TODO: query3d(): change in CO2 over months of survey w/ hybrids in 20,40,60 mile ranges" 

# Actually do stuff after all
try:
    connect()
    queryData()
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

