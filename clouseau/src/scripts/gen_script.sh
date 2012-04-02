#!/bin/sh
cat <<-ENDOFMESSAGE>clouseau
#!/bin/sh
if [ \$# -lt 1 ]
then
   echo "Usage: clouseau <executable> [executable parameters]"
fi
LD_PRELOAD="$1/libclouseau.so" "\$@"
ENDOFMESSAGE

chmod +x clouseau
