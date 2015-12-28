FACTS = [
 	Fact.new('rodic', 'Peter', 'Jano'), 
 	Fact.new('rodic', 'Peter', 'Vlado'), 
 	Fact.new('manzelia', 'Peter', 'Eva'), 
 	Fact.new('rodic', 'Vlado', 'Maria'), 
 	Fact.new('rodic', 'Vlado', 'Viera'), 
 	Fact.new('muz', 'Peter'), 
 	Fact.new('muz', 'Vlado'), 
 	Fact.new('muz', 'Jano'), 
 	Fact.new('zena', 'Maria'), 
 	Fact.new('zena', 'Viera'), 
 	Fact.new('zena', 'Eva')
]
 
RULES = [
	Rule.new('DruhyRodic1', [
      Condition.new('rodic', '?X', '?Y'),
      Condition.new('manzelia', '?X', '?Z')
  ],
  [
      AddAction.new(FACTS, 'rodic', '?Z', '?Y')
  ]),
	Rule.new('DruhyRodic2', [
      Condition.new('rodic', '?X', '?Y'),
      Condition.new('manzelia', '?Z', '?X')
  ],
  [
      AddAction.new(FACTS, 'rodic', '?Z', '?Y')
  ]),
	Rule.new('Otec', [
      Condition.new('rodic', '?X', '?Y'),
      Condition.new('muz', '?X')
  ],
  [
      AddAction.new(FACTS, 'otec', '?X', '?Y')
  ]),
	Rule.new('Matka', [
      Condition.new('rodic', '?X', '?Y'),
      Condition.new('zena', '?X')
  ],
  [
      AddAction.new(FACTS, 'matka', '?X', '?Y')
  ]),
	Rule.new('Surodenci', [
      Condition.new('rodic', '?X', '?Y'),
      Condition.new('rodic', '?X', '?Z'),
      Condition.new('<>', '?Y', '?Z')
  ],
  [
      AddAction.new(FACTS, 'surodenci', '?Y', '?Z')
  ]),
	Rule.new('Brat', [
		  Condition.new('surodenci', '?Y', '?Z'),
		  Condition.new('muz', '?Y')
  ],
  [
      AddAction.new(FACTS, 'brat', '?Y', '?Z')
  ]),
	Rule.new('Stryko', [
		  Condition.new('brat', '?Y', '?Z'),
		  Condition.new('rodic', '?Z', '?X')
  ],
  [
      AddAction.new(FACTS, 'stryko', '?Y', '?X'),
      MessageAction.new('?X', ' ma stryka')
  ])
]

