class Engine
	attr_reader :base, :facts
	
	def initialize(facts, rules)
		@facts = facts
		@rules = rules
	end
	
	def infer
		for rule in @rules
      puts "Vyhodnocujem pravidlo #{rule.name}"
			evaluate_rule(rule)
		end
	end
	
	def evaluate_rule(rule, conditions = rule.conditions, dictionary = {})
		condition = conditions.first
		if condition
      case condition.predicate
        when '<>'
          if condition.arguments.map { |a| dictionary[a] || a }.uniq.count == condition.arguments.count
            evaluate_rule(rule, conditions[1..-1], dictionary)
          end
        else
          for fact in @facts
            result = fact.unify(condition, dictionary)
            evaluate_rule(rule, conditions[1..-1], result) if result
          end
      end
    else
      for action in rule.actions
			  action.execute(dictionary)
      end
		end
	end
end
