class Organism
	attr_reader :cells

	def initialize(cells = nil)
		@directions = []
		unless cells
			@cells = []
			16.times { @cells << rand(0xff) }
			48.times { @cells << 0 }
		else
			@cells = cells
		end
	end
	
	def fitness
		unless @fitness
			machine = Machine.new
			@fitness, @directions = machine.run(@cells)
		end
		@fitness
	end
	
	def mutate!(probability)
		64.times { |c|
			8.times { |i|
				if rand < probability
					if @cells[c] & 2^i > 0
						@cells[c] &= ~(2^i)
					else
						@cells[c] |= 2^i
					end
				end
			}
		}
		@fitness = nil
	end
	
	def self.cross_over(male, female)
		ancestor_a = []
		ancestor_b = []
	
		32.times { |i|
			ancestor_a[i] = female.cells[32 + i] 
			ancestor_a[32 + i] = male.cells[32 + i] 
			
			ancestor_b[i] = female.cells[i] 
			ancestor_b[32 + i] = male.cells[i] 
		}
		
		[Organism.new(ancestor_a), Organism.new(ancestor_b)]
	end
	
	def self.cross_over!(male, female)
		32.times { |i|
			t = male.cells[i]
			male.cells[i] = female.cells[32 + i] 
			female.cells[32 + i]  = t
		}
		[male, female]
	end
	
	def to_s
		@directions.join
	end
end

