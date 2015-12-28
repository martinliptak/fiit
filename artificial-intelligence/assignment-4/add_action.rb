class AddAction < Action
	attr_reader :facts, :name, :objects

	def initialize(facts, name, *objects)
		@facts = facts
		@name = name
		@objects = objects
	end
	
	def execute(dictionary)
    fact = Fact.new(@name, *@objects.map { |o|
      dictionary[o] || o }
    )
    unless @facts.include?(fact)
      puts "Pridavam fakt #{fact}"
      @facts << fact
    end
	end
	
	def to_s
		"(add #{@name} #{@objects.join(" ")})"
	end
end
