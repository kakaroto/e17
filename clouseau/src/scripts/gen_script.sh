#!/bin/sh
cat <<-ENDOFMESSAGE>clouseau_start
#!/bin/sh
if [ \$# -lt 1 ]
then
   echo "Usage: clouseau_start <executable> [executable parameters]"
else
# Start clouseau daemon (will start single instance), then run app
   clouseaud
   LD_PRELOAD="$1/libclouseau.so" \$*
fi
ENDOFMESSAGE

chmod +x clouseau_start
