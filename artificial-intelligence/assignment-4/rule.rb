class Rule
	attr_reader :name, :conditions, :actions

	def initialize(name, conditions, actions)
		@name = name
		@conditions = conditions
    @actions = actions
	end
	
	def to_s
		"#{@name}: #{@conditions.map(&:to_s).join(", ")} => #{@action}"
	end
end
