# /bin/bash

HOST="patch@patchbox.local"

# Copy all the needed files to the raspberry
scp Configuration/asound.conf "$HOST:~" 2>/dev/null
RET=$(($?))
scp Configuration/raciderry.service "$HOST:~" 2>/dev/null
RET=$(($RET+$?))
scp Scripts/install.sh "$HOST:~" 2>/dev/null
RET=$(($RET+$?))

if [[ $RET -ne 0 ]]; then
    echo "Error : could not copy files on the patchbox system" >&2
    echo "Please make sure the host is reachable at $HOST" >&2
    exit 1;
fi

echo "Success"
echo "You can now log into your rpi and run the following command :"
echo "sudo install.sh"