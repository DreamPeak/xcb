#!/bin/bash
clients=`xcb-cli -x show clients`
if test "${clients#*ERR}" != "$clients"
then
    echo "Warning - 'show clients' may not be supported"
    exit 1
fi
num=`echo "$clients" | wc -l`
((num -= 1))
echo "OK - $num clients connected"
exit 0

