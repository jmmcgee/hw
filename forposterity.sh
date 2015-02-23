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
