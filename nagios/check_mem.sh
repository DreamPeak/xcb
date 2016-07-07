#script to check real memory usage
# L.Gill 02/05/06 - V.1.0
# ------------------------------------------
# ########  Script Modifications  ##########
# ------------------------------------------
# Who         When           What
# ---    ----      ----
# LGill         17/05/06  "$percent" lt 1% fix - sed edits dc result beggining with "."
#
#
#!/bin/bash
USAGE="`basename $0` [-w|--warning]<percent free> [-c|--critical]<percent free>"
THRESHOLD_USAGE="WARNING threshold must be greater than CRITICAL: `basename $0` $*"
percent_free=/tmp/mempercent
critical=""
warning=""
STATE_OK=0
STATE_WARNING=1
STATE_CRITICAL=2
STATE_UNKNOWN=3
# print usage
if [[ $# -lt 4 ]]
then
        echo ""
        echo "Wrong Syntax: `basename $0` $*"
        echo ""
        echo "Usage: $USAGE"
        echo ""
        exit 0
fi
# read input
while [[ $# -gt 0 ]]
  do
        case "$1" in
               -w|--warning)
               shift
               warning=$1
        ;;
               -c|--critical)
               shift
               critical=$1
        ;;
        esac
        shift
  done

# verify input
if [[ $warning -eq $critical || $warning -lt $critical ]]
then
        echo ""
        echo "$THRESHOLD_USAGE"
        echo ""
        echo "Usage: $USAGE"
        echo ""
        exit 0
fi
# Total memory available
total=`free -m | head -2 |tail -1 |gawk '{print $2}'`
# Total memory used
used=`free -m | head -2 |tail -1 |gawk '{print $3}'`
# Calc total minus used
free=`free -m | head -2 |tail -1 |gawk '{print $4+$6+$7}'`
# normal values
#echo "$total"MB total
#echo "$used"MB used
#echo "$free"MB free

# make it into % percent free = ((free mem / total mem) * 100)
percent=$[ free * 100 / total]

if [[ "$percent" -le  $critical ]]
        then
                echo "CRITICAL - $free MB ($percent%) Free Memory"
                exit 2
fi
if [[ "$percent" -le  $warning ]]
        then
                echo "WARNING - $free MB ($percent%) Free Memory"
                exit 1
fi
if [[ "$percent" -gt  $warning ]]
        then
                echo "OK - $free MB ($percent%) Free Memory"
                exit 0
fi
