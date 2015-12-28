require "./state"

def find_solution(initial)
	states = []
	states << initial
	loop do
		state = states.pop
		if not state
			return nil
		elsif state.final?
			return state
		else
			state.push_actions(states)
		end
	end
end

srand
[5, 6].each { |size|
	start = Time.now
	puts "Size #{size}x#{size}"
	puts "Solving 1 1"
	puts (find_solution(State.new(size, 0, 0)) || "No solution found")
	4.times { |i|
		x = rand(size)
		y = rand(size)
		puts "Solving #{x + 1} #{y + 1}"
		puts (find_solution(State.new(size, x, y)) || "No solution found") 
	}
	puts "Search time #{(Time.now - start).round}s"
}

