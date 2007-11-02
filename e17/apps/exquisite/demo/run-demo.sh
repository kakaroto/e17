#!/bin/sh

# to avoid needing an installl- lets run this from the src dir
MAIN="../src/bin/exquisite"
WRITE="../src/bin/exquisite-write"

# start up gui - some examples here for different resolutions and the fb
#$MAIN -t ../data/themes/default.edj -x 480 -y 640 &
$MAIN -t ../data/themes/default.edj -x 640 -y 480 &
#$MAIN -t ../data/themes/default.edj -x 1280 -y 1024 &
#$MAIN -fb -fs
# run 'exquisite -h' for more options
echo "I: waiting..."
sleep 1
$WRITE "TICK"
sleep 1
$WRITE "TICK"
$WRITE "MSG Hello World"
sleep 1
$WRITE "MSG Starting up..."
sleep 1
$WRITE "TICK"
sleep 1
$WRITE "TICK"
$WRITE "PROGRESS 5"
sleep 1
$WRITE "PROGRESS 10"
sleep 1
$WRITE "MSG Services starting..."
$WRITE "PROGRESS 20"
sleep 2
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
$WRITE "MSG Unknown setup..."
$WRITE "PULSATE"
sleep 5
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
$WRITE "TICK"
sleep 1
$WRITE "TICK"
sleep 1
$WRITE "TICK"
sleep 1
$WRITE "TICK"
sleep 1
echo "I: quitting..."
# use this to quit now - otherwise quit may take some time - animated maybe
#$WRITE QUIT
$WRITE END
echo "I: done"

exit 0
