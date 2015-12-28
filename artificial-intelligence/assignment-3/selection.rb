class Selection
	attr_reader :organisms

	def initialize(organisms)
		@organisms = organisms
		@roulette_random_max = ((1 + @organisms.count) / 2 * @organisms.count).to_i
	end

	def tournament
		male_a = @organisms[rand(organisms.count)]
		male_b = @organisms[rand(organisms.count)]
		male = male_a.fitness > male_b.fitness ? male_a : male_b
	
		female_a = @organisms[rand(organisms.count)]
		female_b = @organisms[rand(organisms.count)]
		female = female_a.fitness > female_b.fitness ? female_a : female_b
	
		[male, female]
	end

	def roulette
		k = ((-1 + Math.sqrt(1 + 8 * rand(@roulette_random_max))) / 2).to_i.abs
		l = ((-1 + Math.sqrt(1 + 8 * rand(@roulette_random_max))) / 2).to_i.abs
	
		[organisms[@organisms.count - k - 1], organisms[@organisms.count - l - 1]]
	end
end
