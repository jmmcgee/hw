
import java.awt.Point;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Queue;
import java.util.HashSet;
import java.util.PriorityQueue;



public class PerfectAI implements AIModule
{
    private static final double SQRT_2 = Math.sqrt(2.0);

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

    HeuristicPoint start;
    HeuristicPoint end;

    /// Creates the path to the goal.
    public List<Point> createPath(final TerrainMap map)
    {
        // Holds the resulting path
        final ArrayList<Point> path = new ArrayList<Point>();
        Queue<HeuristicPoint> open = new PriorityQueue<HeuristicPoint>();
        HashSet<Long> closed = new HashSet<Long>();
 
        double[][] knownCosts = new double[map.getWidth()][map.getHeight()];
        for(int i = 0; i < map.getWidth(); i++)
            for(int j = 0; j < map.getHeight(); j++)
                knownCosts[i][j] = Double.POSITIVE_INFINITY;

        // Keep track of where we are and add the start point to the open set.
        start = new HeuristicPoint(null, map.getStartPoint(), 0.0, 0.0);
        end = new HeuristicPoint(null, map.getEndPoint(), 0.0, 0.0);
        open.add(start);
        knownCosts[start.x][start.y] = 0.0;

        while(!open.isEmpty())
        {
            // Poll for node to explore
            HeuristicPoint currentPoint = open.poll();
            if(currentPoint.knownCost != knownCosts[currentPoint.x][currentPoint.y])
            	continue;
            if(currentPoint.x == end.x && currentPoint.y == end.y) {
                end = currentPoint;
                break;
            }
            
            // add node to closed list
            closed.add(currentPoint.getLongKey());

            // Retrieve neighbors and update appropriately
            for(Point p : map.getNeighbors(currentPoint)) {
            	double knownCost = currentPoint.knownCost + map.getCost(currentPoint, p);
            	double estimateCost = knownCost + getHeuristic(map, p, end);
            	HeuristicPoint nbor = new HeuristicPoint(currentPoint, p, knownCost, estimateCost);
                // if already in closed set, skip
            	
                if( closed.contains(nbor.getLongKey()) )
                    continue;

                // add to open set if knownCost beats (is less than) current stored knownCost for this point
                if( nbor.knownCost < knownCosts[nbor.x][nbor.y] ) {
                    open.add(nbor);
                    knownCosts[nbor.x][nbor.y] = nbor.knownCost;
                }
            }
        }

	        for(HeuristicPoint node = end; node != null; node = node.parent) {
	            path.add(node);
	        }
	        Collections.reverse(path);
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
