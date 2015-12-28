require "./calendar.rb"

MaxTask = 1000
Tc = 0.1
Tp = 0.015
N = 50

queue = []
current = 0
completed = 0
quantums = 0
latency_sum = 0
queue_sum = 0
unused = 0

random = Random.new
calendar = Calendar.new :condition => Proc.new  {
  completed <= 1000 # podmienka ukoncenia
} 

time = Time.now
puts "Zaciatok simulacie"

# Pouzivatel zada ulohu
event_enter_task = lambda { |n| 
  puts "#{calendar.simtime.round(3)} Terminal #{n} zadava ulohu"
  
   # zaradime do fronty
  queue.push :ts => random.exponential(0.8), :terminal => n, :entered => calendar.simtime
}

# Pouzivatel rozmysla
event_think = lambda { |n|
  puts "#{calendar.simtime.round(3)} Terminal #{n} rozmysla"
  
  # potom zada ulohu
  calendar.enqueue event_enter_task, :exponential => 25, :argument => n 
}

# Planovac
event_scheduler = lambda { 
  next_quantum = Tc
  task = queue.shift
  
  if task and task[:ts] == 0 # uloha skoncena
    puts "#{calendar.simtime.round(3)} Ukoncujem ulohu na #{task[:terminal]}"
    
    latency_sum += calendar.simtime - task[:entered]
    completed += 1
    
    calendar.enqueue event_think, :argument => task[:terminal] # pouzivatel rozmysla dalej
    
    task = queue.shift # dalsia uloha
  end
    
  if task
    if task[:ts] <= Tc - Tp
      puts "#{calendar.simtime.round(3)} Planujem ukoncenie na #{task[:terminal]}"
      
      next_quantum = task[:ts] # po skonceni ulohy sa pusti planovac a uloha bude hned odstranena
      task[:ts] = 0
      queue.unshift(task) # zaciatok fronty
    else
      puts "#{calendar.simtime.round(3)} Planujem ulohu na #{task[:terminal]}"
      
      task[:ts] -= Tc - Tp # z casu ulohy odpocitame a planovac pustime o kvantum
      queue.push(task) # na koniec fronty
    end
  else
    unused += 1
  end
 
  quantums += 1 
  queue_sum += queue.length
  
  calendar.enqueue event_scheduler, :in => next_quantum
}

# CPU
calendar.enqueue event_scheduler

# Terminaly
N.times { |n|
  calendar.enqueue event_think, :argument => n
}

calendar.loop

puts "Koniec simulacie (simulacia trvala #{Time.now - time}s)"
puts
puts "Priemerna doba odozvy: #{(latency_sum.to_f / completed).round(2)}"
puts "Priemerny pocet uloh v rade: #{(queue_sum.to_f / quantums).round(2)}"
puts "Priemerny vytazenie CPU: #{(100 - (unused.to_f / quantums) * 100).round(2)}%"
