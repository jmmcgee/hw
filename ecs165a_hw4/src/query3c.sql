SELECT year, month, (co2 * days / numHouseholds * {0} / value)*100 AS res
FROM
(
    SELECT year, month, days, SUM(trpmiles / epatmpg * '8.887E-9') AS co2
    FROM
    (
        SELECT houseid, personid, tdtrpnum, year, month, vehid, trpmiles
        FROM TravelDay
        WHERE trpmiles >= 0 AND vehid >= 1
    ) trips NATURAL JOIN
    (
        SELECT houseid, vehid, epatmpg
        FROM Vehicle
    ) vehs NATURAL JOIN DaysInMonth
    GROUP BY year, month, days
) perHousehold NATURAL JOIN
(
    SELECT year, month, COUNT(houseid) AS numHouseholds
    FROM 
    (
        SELECT houseid, year, month
        FROM Household
    ) h
    GROUP BY year, month
) households NATURAL JOIN
(
    SELECT year, month, value
    FROM TransportationCO2
    WHERE msn = 'TEACEUS'
) totalCO2
ORDER BY year, month
;

