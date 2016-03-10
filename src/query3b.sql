SELECT CAST(selected.avgFuel AS REAL)/CAST(total.weight AS REAL)
FROM
(
    SELECT SUM(trpmiles * epatmpg * days) AS avgFuel
    FROM (
        -- select trips with miles less than specificed limit
        SELECT houseid, personid, tdtrpnum, month, vehid, trpmiles
        FROM TravelDay
        WHERE vehid >= 1 AND trpmiles < {0} AND trpmiles >= 0
    ) trips NATURAL JOIN
    (
        SELECT houseid, vehid, epatmpg
        FROM Vehicle
    ) vehs NATURAL JOIN DaysInMonth
) selected,
(
    SELECT SUM(trpmiles * days) AS weight
    FROM (
        -- select trips with miles less than specificed limit
        SELECT houseid, personid, tdtrpnum, month, vehid, trpmiles
        FROM TravelDay
        WHERE vehid >= 1 AND trpmiles < {0} AND trpmiles >= 0
    ) trips NATURAL JOIN DaysInMonth
) total
;

