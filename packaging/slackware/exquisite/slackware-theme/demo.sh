#!/bin/sh

MAIN="exquisite -verbose"
WRITE="exquisite-write"

$MAIN -t ./slackware.edj -x 800 -y 600 &
# run 'exquisite -h' for more options
echo "I: waiting up to 30 secs for exquisite..."
$WRITE -wait 30
echo "OK. 1 second, then go"
sleep 1
$WRITE "TIMEOUT 20"
$WRITE "MSG Starting Up"
sleep 2
$WRITE "MSG Services Starting"
$WRITE "PROGRESS 5"
sleep 4
$WRITE "PROGRESS 10"
sleep 1
$WRITE "PROGRESS 15"
$WRITE "MSG Populating the World"
sleep 2
$WRITE "PROGRESS 20"
$WRITE "MSG Curing Cancer"
sleep 3
$WRITE "PROGRESS 25"
sleep 1
$WRITE "MSG Running Text Tests"
sleep 1
$WRITE "MSG Checking applications..."
$WRITE "PROGRESS 40"
sleep 1
$WRITE "PROGRESS 42"
sleep 1
$WRITE "PROGRESS 44"
sleep 1
$WRITE "PROGRESS 46"
sleep 1
$WRITE "PROGRESS 48"
sleep 1
$WRITE "PROGRESS 50"
sleep 1
$WRITE "MSG Configuration..."
$WRITE "PROGRESS 60"
sleep 1
$WRITE "MSG Starting UI services..."
$WRITE "PROGRESS 70"
sleep 1
$WRITE "MSG Syncronising..."
$WRITE "PROGRESS 80"
sleep 1
$WRITE "MSG Almost done."
$WRITE "PROGRESS 90"
sleep 1
$WRITE "MSG Done."
$WRITE "PROGRESS 100"
sleep 1
$WRITE "CLEAR"
echo "I: quitting in 5 seconds..."
# use this to quit now - otherwise quit may take some time - animated maybe
#$WRITE QUIT
$WRITE END
echo "I: done"

exit 0
