
SELECT SUM(trpmiles * epatmpg) AS avgFuel
FROM (
	-- select trips with miles less than specificed limit
	SELECT houseid, personid, tdcaseid, vehid, trpmiles
	FROM TravelDay
	WHERE vehid >= 1 AND trpmiles < {0} AND trpmiles >= 0
) trips JOIN
(
	SELECT houseid, vehid, epatmpg
	FROM Vehicle
) economy
--GROUP BY trpmiles, epatmpg
) totalEcon, (
	SELECT SUM(trpmiles) AS totalMiles
	FROM (
		-- select trips with miles less than specificed limit
		SELECT houseid, personid, tdcaseid, vehid, trpmiles
		FROM TravelDay
		WHERE vehid >= 1 AND trpmiles < {0} AND trpmiles >= 0
	) trips2
) total;
