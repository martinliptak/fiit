import java.lang.*;
import java.util.*;

public class State
{
	private int size, x, y, number;
	private int[] board;

	// vutvorenie noveho stavu
	public State(int size, int x, int y, int number, int[] board)
	{
		this.size = size;
		this.x = x;
		this.y = y;
		this.number = number;
		
		// pri vytvarani prveho stavu si sachovnicu vytvori sam
		// ostatne sachovnice vzniknu kopirovanim v pushNewPositionIfValid()
		if (board != null)
			this.board = board;
		else
			this.board = new int[size * size]; 
			
		// prida novu poziciu
		this.board[x * size + y] = number;
	}
	
	// je konecny?
	public boolean isFinal()
	{
		return number == size * size;
	}
	
	// prida stavy (potomkov) na zasobnik
	public void pushActions(Deque<State> states) 
	{
		// zavola pushNewPositionIfValid() pre vsetky smery kona
		pushNewPositionIfValid(states, this.x - 2, this.y + 1);
		pushNewPositionIfValid(states, this.x - 2, this.y - 1);
		pushNewPositionIfValid(states, this.x + 2, this.y - 1);
		pushNewPositionIfValid(states, this.x + 2, this.y + 1);
		pushNewPositionIfValid(states, this.x + 1, this.y + 2);
		pushNewPositionIfValid(states, this.x + 1, this.y - 2);
		pushNewPositionIfValid(states, this.x - 1, this.y + 2);
		pushNewPositionIfValid(states, this.x - 1, this.y - 2);
	}
	
	
	// vypis konecneho stavu
	public String toString()
	{
		StringBuilder sb = new StringBuilder(127);
		
		for (int j = size - 1; j >= 0; j--)
		{
			for (int i = 0; i < size; i++)
				sb.append(String.format("%2d ", board[i * size + j]));
			sb.append("\n");
		}
			
		return sb.toString();
	}
	
	// prida novy stav na zasobnik, ak to ma zmysel
	private void pushNewPositionIfValid(Deque<State> states, int x, int y)
	{
		if (x >= 0 && x < size && y >= 0 && y < size) // je na sachovnici?
			if (board[x * size + y] == 0) // neboli sme tam?
				states.addFirst(new State(size, x, y, number + 1, board.clone())); // pridame stav
	}
}

