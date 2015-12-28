require './fact'
require './rule'
require './condition'
require './action'
require './add_action'
require './remove_action'
require './message_action'
require './base'
require './engine'

engine = Engine.new(FACTS, RULES)
engine.infer

