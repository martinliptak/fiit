import java.util.*;

public class Main
{
	// najde (alebo nenajde) riesenie
	private static State findSolution(State initial)
	{
		Deque<State> states = new ArrayDeque<State>(); // zasobnik
		int steps = 0;
		
		states.addFirst(initial); // vlozime pociatocny stav
		for (;;)
		{
			/*
			steps++;
			if (steps > 100000000)
				return null; // prekroceny limit iteracii
			*/
				
			State state;
			if (states.size() > 0) 
				state = states.removeFirst(); // vyberieme stav zo zasobnika
			else
				return null; // prázdny zásobník, riesenie neexistuje
				
			if (state.isFinal()) // konečný stav
				return state; // vratime riesenie
			else
				state.pushActions(states); // pridáme potomkov aktualneho stavu
		}
	}
	
	public static void main(String [] args)
	{
		State found;
		Random randomGenerator = new Random();
		
		Integer customX = null, customY = null;
		
		long start = System.currentTimeMillis();
		int size = Integer.parseInt(args[0]);
		
		if (args.length > 1)
		{
			customX = Integer.parseInt(args[1]);
			customY = Integer.parseInt(args[2]);
		}
		
		System.out.println("Size " + size + "x" + size);

		if (customX == null || customY == null)
		{
			// pozicia 1 1
			System.out.println("Solving field " + 1 + " " + 1);
			System.out.println(findSolution(new State(size, 0, 0, 1, null)));
			for (int i = 0; i < 4; i++)
			{
				// nahodne pociatocne pozicie
				int x = randomGenerator.nextInt(size);
				int y = randomGenerator.nextInt(size);
			
				System.out.println("Solving field " + (x + 1) + " " + (y + 1));
				System.out.println(findSolution(new State(size, x, y, 1, null)));
			}
		}	
		else
		{
			// vlastna pozicia
			System.out.println("Solving field " + customX + " " + customY);
			System.out.println(findSolution(new State(size, customX, customY, 1, null)));
		}
			
		long end = System.currentTimeMillis();
		System.out.println("Execution time " + (end - start) + "ms");
	}
}
