#! /bin/bash

DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Copy the alsa configuration file
cp "$DIR/asound.conf" /etc/

# Copy the service configuration file and reload systemd daemon
cp "$DIR/raciderry.service" /etc/systemd/system
systemctl daemon-reload
systemctl enable raciderry
systemctl start raciderry