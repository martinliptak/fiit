require './selection.rb'
require './organism.rb'
require './machine.rb'
require './grid.rb'

populationSize = 100
Generations = 1000
ElitePart = 0.05
AncestorPart = 0.7
SelectionMethod = :roulette
mutationProbability = 0.5

srand

Grid.read_from_stdin

#10.times { |p|
#	mutationProbability = (p + 1) * 0.1
#	total_generations = 0
#	10.times {
		organisms = []
		populationSize.times { organisms << Organism.new }

		Generations.times do |i|
			organisms.sort! { |o, p| p.fitness <=> o.fitness }
	
			best = organisms.first
			puts "Best fitness #{best.fitness} in generation #{i + 1}"
	
			unless best.fitness / 1000 == Grid.treasure_count
				elite_organisms = organisms.take(ElitePart * populationSize)
		
				ancestor_organisms = []
				selection = Selection.new(organisms)
				(AncestorPart * populationSize / 2).to_i.times {
					a = Organism.cross_over(*selection.send(SelectionMethod))
			
					a[0].mutate!(mutationProbability)
					ancestor_organisms << a[0]
			
					a[1].mutate!(mutationProbability)
					ancestor_organisms << a[1] 
				}
		
				organisms = elite_organisms + ancestor_organisms
		
				(populationSize - organisms.count).times {
					organisms << Organism.new
				}
			else
				#total_generations += i + 1
		
				#puts "#{i + 1}\t#{best.fitness}\t#{best.to_s}"
				#break
				puts best.to_s
				exit
			end
		end
#	}
#	puts "#{mutationProbability}\t#{total_generations.to_f / 10}"
#}

