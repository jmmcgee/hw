#!/bin/bash

export LOCAL_DB=~/postgres
export PGHOST=$LOCAL_DB
export PGDATA=$LOCAL_DB/data

alias start_postgres='pg_ctl -w -l ~/postgres_server.log start -o "-c listen_addresses= -c unix_socket_directories=$LOCAL_DB"'
alias stop_postgres='pg_ctl stop'

mkdir -p $LOCAL_DB
initdb
