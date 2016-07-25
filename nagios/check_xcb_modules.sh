#!/bin/bash
modules=`xcb-cli -x show modules`
if test "${modules#*ERR}" != "$modules"
then
    echo "Warning - 'show modules' may not be supported"
    exit 1
fi
num=`echo "$modules" | wc -l`
echo "OK - $num modules loaded"
exit 0

