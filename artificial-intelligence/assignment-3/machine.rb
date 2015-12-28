class Machine
	
	Instructions = {
		0 => :increment,
		1 => :decrement,
		2 => :jump,
		3 => :print
	}
	
	def run(program)
		@ip = 0
		@memory = program.clone
		@grid = Grid.new
		@directions = []
		
		cycles = 0
		while cycles < 500
			break if @ip > 63
			
			instruction = @memory[@ip] >> 6
			address = @memory[@ip] & 0x3f
			
			send Instructions[instruction], address
			
			cycles += 1
		end
		
		[@grid.found_count * 1000 + 1000 - cycles * 2, @directions]
	end
	
	private
	
	def increment(address)
		@memory[address] += 1
		@memory[address] = 0 if @memory[address] > 255
		@ip += 1
	end
	
	def decrement(address)
		@memory[address] -= 1
		@memory[address] = 255 if @memory[address] < 0
		@ip += 1
	end
	
	def jump(address)
		@ip = address
	end
	
	def print(address)
		@ip += 1
		
		value = @memory[address]
		
		ones = 0
		8.times {
			ones += value & 1
			value >>= 1
		}
		
		direction = if ones <= 2
			:h
		elsif ones <= 4
			:d
		elsif ones <= 6
			:p
		else
			:l
		end
		
		@directions << direction
		
		halt if not @grid.move(direction) or @grid.found_all?
	end
	
	def halt
		@ip = 65
	end
end

