
import java.awt.Point;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Queue;
import java.util.HashMap;
import java.util.HashSet;
import java.util.PriorityQueue;



public class BiDijkstraAI implements AIModule
{
    static public class HeuristicPoint extends Point implements Comparable<HeuristicPoint> {
        public HeuristicPoint(HeuristicPoint parent, Point p, double knownCost, double estimateCost){
            super(p);
            this.parent = parent;
            this.knownCost = knownCost;
            this.estimateCost = estimateCost;
        }

        @Override
        public int compareTo(HeuristicPoint p2){
            if(estimateCost < p2.estimateCost) return -1;
            else if(estimateCost > p2.estimateCost) return 1;
            else return 0;
        }

        public long getLongKey() {
            return ((long)x) | ((long)y)<<32;
        }

        public double knownCost;
        public double estimateCost;

        HeuristicPoint parent;
    }

    HeuristicPoint start = null;
    HeuristicPoint midA = null;
    HeuristicPoint midB = null;
    HeuristicPoint end = null;

    /// Creates the path to the goal.
    public List<Point> createPath(final TerrainMap map)
    {
        // Holds the resulting path
        final ArrayList<Point> path = new ArrayList<Point>();
        Queue<HeuristicPoint> openA = new PriorityQueue<HeuristicPoint>();
        Queue<HeuristicPoint> openB = new PriorityQueue<HeuristicPoint>();
        HashMap<Long,HeuristicPoint> closedA = new HashMap<Long,HeuristicPoint>();
        HashMap<Long,HeuristicPoint> closedB = new HashMap<Long,HeuristicPoint>();
 
        double[][] knownCostsA = new double[map.getWidth()][map.getHeight()];
        double[][] knownCostsB = new double[map.getWidth()][map.getHeight()];
        for(int i = 0; i < map.getWidth(); i++)
            for(int j = 0; j < map.getHeight(); j++) {
                knownCostsA[i][j] = Double.POSITIVE_INFINITY;
                knownCostsB[i][j] = Double.POSITIVE_INFINITY;
            }

        // Keep track of where we are and add the start point to the open set.
        start = new HeuristicPoint(null, map.getStartPoint(), 0.0, 0.0);
        end = new HeuristicPoint(null, map.getEndPoint(), 0.0, 0.0);
        openA.add(start);
        openB.add(end);
        knownCostsA[start.x][start.y] = 0.0;
        knownCostsB[end.x][end.y] = 0.0;

        while(!openA.isEmpty() && !openB.isEmpty())
        {
            // Poll for node to explore
            HeuristicPoint currentPointA = openA.poll();
            HeuristicPoint currentPointB = openB.poll();
            while(currentPointA.knownCost != knownCostsA[currentPointA.x][currentPointA.y])
            	currentPointA = openA.poll();
            while(currentPointB.knownCost != knownCostsB[currentPointB.x][currentPointB.y])
            	currentPointB = openB.poll();
            
            // add node to closed list
            closedA.put(currentPointA.getLongKey(), currentPointA);
            closedB.put(currentPointB.getLongKey(), currentPointB);

            // A* A and A* B meet at currentPointA, finished
            HeuristicPoint temp;
            if( (temp = closedB.get(currentPointA.getLongKey())) != null ) {
            	midA = currentPointA;
            	midB = temp;
            	break;
            }
            // A* A and A* B meet at currentPointB, finished
            if( (temp = closedA.get(currentPointB.getLongKey())) != null ) {
            	midA = temp;
            	midB = currentPointB;
            	break;
            }
            	
            // Retrieve neighbors and update appropriately
            // A: start -> end
            for(Point p : map.getNeighbors(currentPointA)) {
            	double knownCost = currentPointA.knownCost + map.getCost(currentPointA, p);
            	double estimateCost = knownCost + getHeuristic(map, p, end);
            	HeuristicPoint nbor = new HeuristicPoint(currentPointA, p, knownCost, estimateCost);
            	
                // if already in closed set, skip
            	if(closedA.get(nbor.getLongKey()) != null)
            		continue;

                // add to open set if knownCost beats (is less than) current stored knownCost for this point
                if( nbor.knownCost < knownCostsA[nbor.x][nbor.y] ) {
                    openA.add(nbor);
                    knownCostsA[nbor.x][nbor.y] = nbor.knownCost;
                }
            }
            
            // Retrieve neighbors and update appropriately
            // B: end -> start
            for(Point p : map.getNeighbors(currentPointB)) {
            	double knownCost = currentPointB.knownCost + map.getCost(p, currentPointB);
            	double estimateCost = knownCost + getHeuristic(map, start, p);
            	HeuristicPoint nbor = new HeuristicPoint(currentPointB, p, knownCost, estimateCost);
            	
                // if already in closed set, skip
            	if(closedB.get(nbor.getLongKey()) != null)
                    continue;

                // add to open set if knownCost beats (is less than) current stored knownCost for this point
                if( nbor.knownCost < knownCostsB[nbor.x][nbor.y] ) {
                    openB.add(nbor);
                    knownCostsB[nbor.x][nbor.y] = nbor.knownCost;
                }
            }
        }

        // Back trace A* A from mid back to start
        for(HeuristicPoint node = midA; node != null; node = node.parent) {
            path.add(node);
        }
        Collections.reverse(path);
        // Back trace A* B from mid back to end
        for(HeuristicPoint node = midB.parent; node != null; node = node.parent) {
            path.add(node);
        }
        return path;
    }

    private double getHeuristic(final TerrainMap map, final Point pt1, final Point pt2)
    {
    	return getExponentialHeuristic(map,pt1,pt2);
    	//return getDivisiveHeuristic(map,pt1,pt2);
    }

    // Cost 1: Math.exp(getTile(p2) - getTile(p1));
    private double getExponentialHeuristic(final TerrainMap map, final Point pt1, final Point pt2)
    {
    	// 1 + 1 < e + e^-1 ; 1 + 1 < e^2 + e^ ; 5 < 4e^-4 + 1
    	// essence of heuristic := take difference in height, break into as many increments as it takes to get to the goal
    	final double heightDiff = map.getTile(pt2) - map.getTile(pt1);
    	final double distance = Math.max(Math.abs(pt2.y-pt1.y), Math.abs(pt2.x-pt1.x));

    	return distance*Math.exp(heightDiff/distance);
    }
    
    // Cost 2: (getTile(p2) / (getTile(p1) + 1));
    private double getDivisiveHeuristic(final TerrainMap map, final Point pt1, final Point pt2)
    {
    	final double heightRatio = map.getTile(pt2) / (map.getTile(pt1) + 1);
    	final double distance = Math.max(Math.abs(pt2.y-pt1.y), Math.abs(pt2.x-pt1.x));
    	
		return Math.pow(heightRatio, 1.0/distance);
    }

}
