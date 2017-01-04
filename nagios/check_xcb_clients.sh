#!/bin/bash
#
# Copyright (c) 2013-2017, Dalian Futures Information Technology Co., Ltd.
#
# Xiaoye Meng <mengxiaoye at dce dot com dot cn>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

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

