#!/bin/sh

# to avoid needing an installl- lets run this from the src dir
MAIN="../src/bin/exquisite -verbose"
WRITE="../src/bin/exquisite-write"

# start up gui - some examples here for different resolutions and the fb
$MAIN -t ../data/themes/default.edj -x 480 -y 640 &
#$MAIN -t ../data/themes/default.edj -x 640 -y 480 &
#$MAIN -t ../data/themes/default.edj -x 240 -y 320 &
#$MAIN -t ../data/themes/default.edj -x 1280 -y 1024 &
#$MAIN -fb -fs
# run 'exquisite -h' for more options
echo "I: waiting up to 30 secs for exquisite..."
$WRITE -wait 30
echo "OK. 1 second, then go"
sleep 1
$WRITE "TIMEOUT 5"
$WRITE "TITLE Welcome to Exquisite"
sleep 1
$WRITE "TICK"
sleep 1
$WRITE "TICK"
sleep 1
$WRITE "MSG Starting Up"
$WRITE "PULSATE"
sleep 4
$WRITE "MSG Services Starting"
$WRITE "PROGRESS 5"
sleep 4
$WRITE "TEXT Random Number Generator"
sleep 2
$WRITE "SUCCESS OK"
$WRITE "PROGRESS 10"
$WRITE "TEXT Perpetual Motion Machine"
sleep 1
$WRITE "FAILURE FAIL"
$WRITE "PROGRESS 15"
$WRITE "TEXT Populating the World"
sleep 2
$WRITE "SUCCESS OK"
$WRITE "PROGRESS 20"
$WRITE "TEXT Curing Cancer"
sleep 3
$WRITE "FAILURE FAIL"
$WRITE "PROGRESS 25"
sleep 1
$WRITE "MSG Running Text Tests"
$WRITE "TEXT This is an"
$WRITE "TEXT example of"
$WRITE "TEXT text that"
$WRITE "TEXT doesn't have"
$WRITE "TEXT any status."
$WRITE "TEXT And some more"
$WRITE "TEXT to show how"
$WRITE "TEXT the text can"
$WRITE "TEXT overflow."
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
$WRITE "TICK"
sleep 1
$WRITE "TICK"
sleep 1
$WRITE "TICK"
sleep 1
$WRITE "TICK"
sleep 1
echo "I: quitting in 5 seconds..."
# use this to quit now - otherwise quit may take some time - animated maybe
#$WRITE QUIT
#$WRITE END
echo "I: done"

exit 0
