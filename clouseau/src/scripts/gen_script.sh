#!/bin/sh
cat <<-ENDOFMESSAGE>clouseau_start
#!/bin/sh
if [ \$# -lt 1 ]
then
   echo "Usage: clouseau_start <executable> [executable parameters]"
else
# Start clouseau daemon (will start single instance), then run app
   clouseaud
   LD_PRELOAD="$1/libclouseau.so" "\$@"
fi
ENDOFMESSAGE

cat <<-ENDOFMESSAGE>clouseau
#!/bin/sh
if [ \$# -gt 0 ]
then
   # Start clouseau daemon (will start single instance), then run app
   clouseau_start "\$@" &
fi

clouseau_client
ENDOFMESSAGE

chmod +x clouseau
chmod +x clouseau_start
