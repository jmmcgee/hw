-- calculate the percent of people that travel less than %%s miles
SELECT CAST(countPeopleTraveled AS REAL)/CAST(population AS REAL) * 100
FROM (
    -- get the number of people that travel less than %%s miles
    SELECT COUNT(*) AS countPeopleTraveled
    FROM (
        SELECT houseid, personid, SUM(trpmiles) AS miles
        FROM TravelDay
        GROUP BY houseid, personid
    ) peopleTraveled
    WHERE miles < %s
) n, (
    -- get the number of people given in represntative sample of US population
    SELECT COUNT( (houseid,personid) ) AS population
    FROM Person
) t;
