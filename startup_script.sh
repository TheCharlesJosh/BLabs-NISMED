#!/bin/bash
echo "FINISHED" > /home/root/startedup.txt
/etc/init.d/env_set.sh
/home/root/lcd_ui_test
# shutdown now