
import java.awt.Point;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Queue;
import java.util.HashMap;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.Iterator;
import java.util.Set;


public class PerfectAI implements AIModule
{
    int nsize;
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

    /// Creates the path to the goal.
    public List<Point> createPath(final TerrainMap map)
    {
        // Holds the resulting path
        final ArrayList<Point> path = new ArrayList<Point>();
        Queue<HeuristicPoint> open = new PriorityQueue<HeuristicPoint>();
        HashMap<Long, Boolean> closed = new HashMap<Long, Boolean>();
 
        double[][] knownCosts = new double[map.getWidth()][map.getHeight()];
        for(int i = 0; i < map.getWidth(); i++)
            for(int j = 0; j < map.getHeight(); j++)
                knownCosts[i][j] = Double.POSITIVE_INFINITY;

        // Keep track of where we are and add the start point to the open set.
        final Point start = map.getStartPoint();
        HeuristicPoint end;
        open.add(new HeuristicPoint(null, map.getStartPoint(), 0.0, 0.0));
        knownCosts[start.x][start.y] = 0.0;

        // Keep moving horizontally until we match the target.
        while(true)
        {
            // Open is empty -> error
            if(open.isEmpty()){
                System.out.println("EMPTYPATH");
                return path;
            }

            HeuristicPoint currentPoint = open.poll();
            // check to make sure is not deprecated instance of point (inferior knownCost)
            if(currentPoint.knownCost > knownCosts[currentPoint.x][currentPoint.y])
                continue;
            if(currentPoint.knownCost < knownCosts[currentPoint.x][currentPoint.y])
                System.out.println("Something is very wrong. known cost less than should be.");
            
            if(currentPoint.x == map.getEndPoint().x && currentPoint.y == map.getEndPoint().y) {
                end = currentPoint;
                break;
            }

            Point[] nbors = map.getNeighbors(currentPoint);
            nsize = map.getNeighbors(currentPoint).length;
            HeuristicPoint[] neighbors = new HeuristicPoint[nbors.length];
            for(int i = 0; i < nbors.length; i++) {
                double knownCost = map.getCost(currentPoint,nbors[i]) + currentPoint.knownCost;
                double estimateCost = knownCost + getHeuristic(map, currentPoint, nbors[i]);
                neighbors[i] = new HeuristicPoint(currentPoint, nbors[i], knownCost, estimateCost);
            }

            closed.put(currentPoint.getLongKey(), true);

            for(int i = 0; i < neighbors.length; ++i) {
                // if already in closed set, skip
                if( closed.get(neighbors[i].getLongKey()) != null )
                    continue;

                // add to open set if knownCost beats (is less than) current stored knownCost for this point
                if( neighbors[i].knownCost < knownCosts[neighbors[i].x][neighbors[i].y] ) {
                    open.add(neighbors[i]);
                    knownCosts[neighbors[i].x][neighbors[i].y] = neighbors[i].knownCost;
                }
            }
        }

        // TODO: construct path
        for(HeuristicPoint node = end; node != null; node = node.parent) {
            path.add(node);
        }
        Collections.reverse(path);
        System.out.println("Num Neighbors: "+ nsize);
        // We're done!  Hand it back.
        return path;
    }

    // Exponential of the height difference


    // Cost 1: Math.exp(getTile(p2) - getTile(p1));
    // Cost 2: (getTile(p2) / (getTile(p1) + 1));
    private double getHeuristic(final TerrainMap map, final Point pt1, final Point pt2)
    {
        return 1;
    }

}
