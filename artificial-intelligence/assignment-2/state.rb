class State

	def initialize(size, x, y, number = 0, board = [])
		@size = size
		@x = x
		@y = y
		@number = number
		@board = board
		@board[x * size + y] = number
	end
	
	def final?
		@number == @size * @size - 1
	end
	
	def push_actions(states)
		positions = [[@x - 2, @y + 1], [@x - 2, @y - 1], [@x + 2, @y + 1], [@x + 2, @y - 1], 
				[@x + 1, @y + 2], [@x + 1, @y - 2], [@x - 1, @y + 2], [@x - 1, @y - 2]]
	
		for x, y in positions
			if (0...@size).include?(x) and (0...@size).include?(y) and not @board[x * @size + y]
				states << State.new(@size, x, y, @number + 1, @board.dup)
			end
		end
	end
	
	def to_s
		s = ""
		(@size - 1).downto(0) { |y|
			s << "\n"
			@size.times { |x|
				s << (@board[x * @size + y] ? ('a'.ord + @board[x * @size + y]).chr : '_')
			}
		}
		s
	end
end

