require 'set'

class Network
	Nodes = 1000
	ShortestPathSamples = 100
	HubSamples = 5
	Infinity = 1000000
	
	def initialize
		@nodes = Array.new(Nodes) {
			Set.new
		}
	end
	
	#
	# Vypisy
	#
	def print_nodes
		@nodes.each_index { |i|
			puts "#{i} => #{@nodes[i].map(&:to_s).join(', ')}"
		}
	end
	
	def print_stats
		puts "Pocet vrcholov podla spojeni"
		
		stats = Hash.new(0)
		@nodes.each { |node| stats[node.size] += 1 }
		stats.sort.each { |size, count|
			puts "#{size} #{count}"
		}
	end
	
	def print_shortest_path
		puts "Priemerna najkratsia cesta"
		
		total = 0
		count = 0
		@nodes.sample(ShortestPathSamples).each_index { |i|
			path = dijkstra(i)
			if path > 0
				total += path
				count += 1
			end
		}
		
		puts (total.to_f / count).round(2)
	end

	#
	# Spojenia
	#
	def random_meeting(node = rand(Nodes))
		connect(node, rand(Nodes))
	end
	
	def foaf_meeting(node = rand(Nodes))
		friend = sample_from_set(@nodes[node])
		return unless friend
		
		friend_of_a_friend = sample_from_set(@nodes[friend])
		return if not friend_of_a_friend
		
		connect(node, friend_of_a_friend)
	end
	
	def hub_meeting(node = rand(Nodes))
		hub = @nodes.index(@nodes.sort_by(&:size).reverse.take(HubSamples).sample)
		return unless hub
		
		connect(node, hub)
	end
	
	private
	
	def connect(a, b)
		unless a == b
			@nodes[a] << b
			@nodes[b] << b
		end
	end
	
	def sample_from_set(set)
		set.to_a.sample
	end
	
	def dijkstra(source)
		visited = Array.new(Nodes, false)
		paths = Array.new(Nodes, Infinity)
		nodes = []
		
		paths[source] = 0
		nodes << source
		
		loop {
			minimal = nodes.min_by { |x| paths[x] }
			break unless minimal
			
			nodes.delete(minimal)
		
			visited[minimal] = true
			@nodes[minimal].each { |target|
				if not visited[target] and paths[target] > paths[minimal] + 1
					paths[target] = paths[minimal] + 1
					nodes << target
				end
			}
		}
		
		reachable = paths.select { |p| p > 0 and p < Infinity }
		if reachable.size > 0
			reachable.reduce(:+).to_f / reachable.size
		else
			0.0
		end
	end
end

network = Network.new

750.times { network.random_meeting }
1000.times { network.foaf_meeting }

network.print_stats
network.print_shortest_path

