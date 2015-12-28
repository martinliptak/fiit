class Condition
	attr_reader :predicate, :arguments
	
	def initialize(predicate, *arguments)
		@predicate = predicate
		@arguments = arguments
	end
	
	def to_s
		"(#{@predicate} #{@arguments.join(" ")})"
	end
end
