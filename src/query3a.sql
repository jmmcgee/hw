-- calculate the percent of people that travel less than x miles
SELECT CAST(selected.sumDays AS REAL)/CAST(total.sumDays AS REAL) * 100
FROM (
    -- get the number of people that travel less than x miles
    SELECT SUM(days) AS sumDays
    FROM (
        SELECT houseid, personid, days
        FROM (
            SELECT houseid, personid, SUM(trpmiles) AS miles
            FROM TravelDay
            WHERE trpmiles >= 0
            GROUP BY houseid, personid
        ) MilesTraveled NATURAL JOIN DaysInMonth
        WHERE miles < %s
    ) d1
) selected, (
    -- get the number of people that travel less than x s miles
    SELECT SUM(days) AS sumDays
    FROM (
        SELECT houseid, personid, days
        FROM (
            SELECT houseid, personid, SUM(trpmiles) AS miles
            FROM TravelDay
            WHERE trpmiles >= 0
            GROUP BY houseid, personid
        ) MilesTraveled NATURAL JOIN DaysInMonth
    ) d2
    ---- get the number of people given in represntative sample of US population
    --SELECT COUNT( (houseid,personid) ) AS population
    --FROM Person
) total;
