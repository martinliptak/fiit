require 'scanf.rb'

class Grid
	attr_reader :found_count
	
	def initialize
		@x = @@start_x
		@y = @@start_y
		
		@found_count = 0
		@hidden = {}
		@@treasures.each { |t| 
			@hidden[t] = 1
		}
	end
	
	def move(direction)
		x, y = case direction
			when :h
				[@x, @y - 1]
			when :d
				[@x, @y + 1]
			when :p
				[@x + 1, @y]
			when :l
				[@x - 1, @y]
		end
		
		if (0...@@size_x).include?(x) && (0...@@size_y).include?(y)
			@x = x
			@y = y
			if @hidden[[x, y]]
				@hidden[[x, y]] = nil
				@found_count += 1
			end
			true
		else
			false
		end
	end
	
	def found_all?
		@hidden.empty?
	end

	def self.read_from_stdin
		@@size_x, @@size_y = scanf("%d %d\n")
		@@start_x, @@start_y = scanf("%d %d\n")
		@@treasure_count = scanf("%d\n").first
		@@treasures = []
		@@treasure_count.times {
			@@treasures << scanf("%d %d\n")
		}
	end
	
	def self.treasure_count
		@@treasure_count
	end

	def self.to_s
		s = ''
		@@size_y.times { |y|
			@@size_x.times { |x|
				if @@treasures.find { |t| t[0] == x && t[1] == y }
					s += 'X'
				elsif x == @@start_x && y == @@start_y
					s += 'S'
				else
					s += '-'
				end
			}
			s += "\n"
		}
		s
	end
end

