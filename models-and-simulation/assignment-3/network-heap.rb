require 'set'
require 'algorithms'

class Network
	Nodes = 1000
	ShortestPathSamples = 100
	Infinity = 1000000
	
	def initialize
		@nodes = Array.new(Nodes) {
			Set.new
		}
	end
	
	def print_nodes
		@nodes.each_index { |i|
			puts "#{i} => #{@nodes[i].map(&:to_s).join(', ')}"
		}
	end
	
	def print_stats
		stats = Hash.new(0)
	
		@nodes.each { |node| stats[node.size] += 1 }
		
		puts "Pocet vrcholov podla spojeni"
		stats.sort.each { |size, count|
			puts "#{size} #{count}"
		}
	end
	
	def print_shortest_path
		total = 0
		@nodes.sample(ShortestPathSamples).each_index { |i|
			total += dijkstra(i)
		}
		
		puts "Priemerna najkratsia cesta"
		puts (total.to_f / ShortestPathSamples).round(2)
	end

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
	
	def dijkstra(source, &lambda)
		visited = Array.new(Nodes, false)
		paths = Array.new(Nodes, Infinity)
		
		heap = Containers::Heap.new { |x, y|
			paths[x] < paths[y]
		}
		heap.push(source)
		
		paths[source] = 0
		loop {
			minimal = heap.pop
			if false
			minimal = nil
			paths.each_index { |i| 
				if not visited[i] and paths[i] < Infinity
					if not minimal or paths[i] < paths[minimal] 
						minimal = i
					end
				end
			}
			end
			break unless minimal
			
			visited[minimal] = true
			@nodes[minimal].each { |target|
				unless visited[target]
					if paths[target] > paths[minimal] + 1
						paths[target] = paths[minimal] + 1
						heap.delete(target)
						heap.push(target)
					end
				end
			}
		}
		
		reachable = paths.select { |p| p < Infinity }
		reachable.reduce(:+).to_f / reachable.size
	end
end

network = Network.new

10000.times { network.random_meeting }
1000.times { network.foaf_meeting }

network.print_shortest_path

