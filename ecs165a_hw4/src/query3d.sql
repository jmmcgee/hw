--SELECT year, month, ((evCO2 - gasCO2) * days / numHouseholds * {0} / 1000000) AS changeCO2
--FROM
--(
--    SELECT year, month, days, (evEVCO2 + evGasCO2) AS evCO2
--    FROM
--    (
--        -- calculate the co2 generated by ev mileage portion of hybrid
--        SELECT year, month, days, SUM( (trpmiles - {1}) / (epatmpg * '0.090634441') * (elecCO2/elec)) AS evEVCO2
--        FROM
--        (
--            SELECT houseid, personid, tdtrpnum, year, month, vehid, trpmiles
--            FROM TravelDay
--            WHERE trpmiles >= {1} AND vehid >= 1
--        ) trips NATURAL JOIN
--        (
--            SELECT houseid, vehid, epatmpg
--            FROM Vehicle
--        ) vehs NATURAL JOIN DaysInMonth NATURAL JOIN
--        (
--            -- correlate with total CO2 generated by electricity in each month
--            SELECT year, month, value AS elecCO2
--            FROM ElectricityCO2
--        ) eia2 NATURAL JOIN
--        (
--            -- correlate with total (net) elecricty generated in each month
--            SELECT year, month, value AS elec
--            FROM ElectricityCO2
--        ) eia3 
--        GROUP BY year, month, days
--    ) perHouseholdEV NATURAL JOIN
--    (
--        -- calculate the co2 generated by gas mileage portion of hybrid (in metric tons of CO2)
--        SELECT year, month, days, SUM( (trpmiles - {1}) / (epatmpg * '8.887E-3')) AS EVGasCO2
--        FROM
--        (
--            SELECT houseid, personid, tdtrpnum, year, month, vehid, trpmiles
--            FROM TravelDay
--            WHERE trpmiles >= {1} AND vehid >= 1
--        ) trips NATURAL JOIN
--        (
--            SELECT houseid, vehid, epatmpg
--            FROM Vehicle
--        ) vehs NATURAL JOIN DaysInMonth
--        GROUP BY year, month, days
--    ) perHouseholdEVTemp
--) perHouseholdEV NATURAL JOIN
--(
--    -- calculate the co2 generated by gas-only trip (in metric tons of CO2)
--    SELECT year, month, days, SUM( trpmiles / epatmpg * '8.887E-3') AS gasCo2
--    FROM
--    (
--        SELECT houseid, personid, tdtrpnum, year, month, vehid, trpmiles
--        FROM TravelDay
--        WHERE trpmiles >= 1 AND vehid >= 1
--    ) trips NATURAL JOIN
--    (
--        SELECT houseid, vehid, epatmpg
--        FROM Vehicle
--    ) vehs NATURAL JOIN DaysInMonth
--    GROUP BY year, month, days
--) perHouseholdGas NATURAL JOIN
--(
--    -- correlate with number of households present in survey in each month
--    SELECT year, month, COUNT(houseid) AS numHouseholds
--    FROM 
--    (
--        SELECT houseid, year, month
--        FROM Household
--    ) h
--    GROUP BY year, month
--) households NATURAL JOIN
--(
--    -- correlate with total CO2 generated by transportation in each month
--    SELECT year, month, value
--    FROM TransportationCO2 AS transCO2
--) eia1 NATURAL JOIN
--(
--    -- correlate with total CO2 generated by electricity in each month
--    SELECT year, month, value AS elecCO2
--    FROM ElectricityCO2
--) eia2 NATURAL JOIN
--(
--    -- correlate with total (net) elecricty generated in each month
--    SELECT year, month, value AS elec
--    FROM ElectricityCO2
--) eia3
--ORDER BY year, month
--;
--
