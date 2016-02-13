
import java.awt.Point;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Queue;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.Iterator;
import java.util.Set;


public class PerfectAI implements AIModule
{
    static public class HeuristicPoint extends Point implements Comparable<HeuristicPoint> {
    	public HeuristicPoint(Point p) {
    		super(p);
    	}
        public HeuristicPoint(HeuristicPoint parent, Point p, double knownCost, double estimateCost){
            super(p);
            this.parent = parent;
            this.knownCost = knownCost;
            this.estimateCost = estimateCost;
        }

        @Override
        public int compareTo(HeuristicPoint p2){
            if(knownCost < p2.knownCost) return -1;
            else if(knownCost > p2.knownCost) return 1;
            else return 0;
        }
        public long getLongKey() {
            return ((long)x) | ((long)y)<<32;
        }

        public double knownCost;
        public double estimateCost;

        HeuristicPoint parent;
    }

    /// Creates the path to the goal.
    public List<Point> createPath(final TerrainMap map)
    {
        // Holds the resulting path
        final ArrayList<Point> path = new ArrayList<Point>();
        Queue<HeuristicPoint> open = new PriorityQueue<HeuristicPoint>();
        HashSet<Long> closed = new HashSet<Long>();
 
        double[][] knownCosts = new double[map.getWidth()][map.getHeight()];
        double[][] estimateCosts = new double[map.getWidth()][map.getHeight()];
        for(int i = 0; i < map.getWidth(); i++) {
            for(int j = 0; j < map.getHeight(); j++) {
                knownCosts[i][j] = Double.POSITIVE_INFINITY;
                estimateCosts[i][j] = Double.POSITIVE_INFINITY;
            }
        }

        // Keep track of where we are and add the start point to the open set.
        final HeuristicPoint start = new HeuristicPoint(null, map.getStartPoint(), 0.0, 0.0);
        HeuristicPoint end = new HeuristicPoint(map.getEndPoint());
        open.add(start);
        knownCosts[start.x][start.y] = 0.0;
        estimateCosts[start.x][start.y] = getHeuristic(map,start,end);

        // Keep moving horizontally until we match the target.
        while(!open.isEmpty())
        {
            // Poll for node to explore
            HeuristicPoint currentPoint = open.poll();
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
                    estimateCosts[nbor.x][nbor.y] = nbor.estimateCost;
                }
            }
        }

        // TODO: construct path
        for(HeuristicPoint node = end; node != null; node = node.parent) {
            path.add(node);
        }
        Collections.reverse(path);
        return path;
    }

    // Exponential of the height difference


    // Cost 1: Math.exp(getTile(p2) - getTile(p1));
    // Cost 2: (getTile(p2) / (getTile(p1) + 1));
    private double getHeuristic(final TerrainMap map, final Point pt1, final Point pt2)
    {
        return 0.0;
    }

}
