#!/bin/bash
./symfony doctrine:build-model
symfony cc
./symfony doctrine:build-sql exchange

mysql -u exchange -pexchange < data/sql/reload.sql
mysql -u exchange -pexchange exchange < data/sql/schema.sql
./symfony doctrine:data-load exchange
