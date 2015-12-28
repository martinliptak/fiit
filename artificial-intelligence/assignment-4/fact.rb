class Fact
	attr_reader :name, :objects

	def initialize(name, *objects)
		@name = name
		@objects = objects
	end
	
	def unify(condition, dictionary)
		bindings = dictionary.clone
	
		if condition.predicate == @name
			if condition.arguments.count == @objects.count
				condition.arguments.count.times do |i|
					if variable?(condition.arguments[i])
						if variable?(@objects[i])
							;
						else
							if bindings[condition.arguments[i]]
								return false if bindings[condition.arguments[i]] != @objects[i]
							else
								bindings[condition.arguments[i]] = @objects[i]
							end
						end
					else 
						if variable?(objects[i])
							if bindings[@objects[i]]
								return false if bindings[@objects[i]] != condition.arguments[i]
							else
								bindings[@objects[i]] = condition.arguments[i]
							end
						else
							if condition.arguments[i] == @objects[i]
								;
							else
								return false
							end
						end
					end
				end
			end
		else
			return false
		end
		
		bindings
  end

  def ==(fact)
    return false if @name != fact.name
    return false if @objects.count != fact.objects.count
    @objects.count.times { |i|
      return false if @objects[i] != fact.objects[i]
    }
    true
  end
	
	def to_s
		"(#{@name} #{@objects.join(" ")})"
	end
	
	private
	
	def variable?(argument)
		argument[0..0] == "?"
	end
end
