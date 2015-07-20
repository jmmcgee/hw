echo '########################'
echo '##### results.txt ######'
echo '########################'
echo

MAXBUFFER=0
echo "====================="
echo MAXBUFFER=${MAXBUFFER}
echo
for x in 0.1 0.25 0.4 0.55 0.65 0.8 0.9; do
    echo ", ARRIVAL_RATE=$x";
    python phase1.py $MAXBUFFER 1 $x;
    echo 
done;
echo "====================="
echo

for MAXBUFFER in 1 20 50; do
    echo "====================="
    echo MAXBUFFER=${MAXBUFFER}
    echo
    for x in 0.2 0.4 0.6 0.8 0.9; do
        echo ", ARRIVAL_RATE=$x";
        python phase1.py $MAXBUFFER 1 $x;
        echo 
    done;
    echo "====================="
    echo
done
