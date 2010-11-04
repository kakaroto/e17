#!/bin/sh

killall -q server
sleep 1

./server & disown
sleep 1

HOST="localhost:4444"
DATA="Hello world!"
SHA1=`php << EOF
<?php echo sha1("$DATA"); ?>
EOF`

curl -s -u bob@test.cz:qwe --no-keepalive -o test.post -A '' -H 'Content-Type: text/plain' -k --data-binary "$DATA" https://$HOST/attach/$SHA1
curl -s -u bob@test.cz:qwe --no-keepalive -o test.get -A '' -k https://$HOST/attach/$SHA1
