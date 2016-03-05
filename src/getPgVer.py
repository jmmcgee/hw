#!/usr/bin/python2
# -*- coding: utf-8 -*-

import psycopg2
import sys
import os

con = None
db = 'postgres'
host = '/home/' + os.environ['USER'] + '/postgres'

try:
    #con = psycopg2.connect(database='postgres', host='localhost:5432')
    con = psycopg2.connect(database=db, host=host)
    cur = con.cursor()
    cur.execute('SELECT version()')
    ver = cur.fetchone()
    print ver

except psycopg2.DatabaseError, e:
    print 'Error %s' % e
    sys.exit(1)

finally:
    if con:
        con.close()
