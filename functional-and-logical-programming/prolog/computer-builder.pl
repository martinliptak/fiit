%
% Martin Liptak
%

% Toto treba spustit
zadanie :-
	write('Component database'), nl, 
	menu.

%
% Pouzivatelske rozhranie
%

% Hlavny cyklus
menu :-
	repeat, 
	nl,
	write('1) Add component'), nl, 
	write('2) Remove component'), nl, 
	write('3) Show component'), nl, 
	write('4) Add system'), nl, 
	write('5) Remove system'), nl, 
	write('6) Show system'), nl, 
	write('7) Load from file'), nl, 
	write('8) Save to file'), nl, 
	write('9) List database'), nl, 
	write('0) Exit'), nl, nl, 
	write('Your number: '), 
	get(Key),
	(
		(Key < 48 ; Key > 57),
		write('Command is invalid.'),
		fail,nl
	;
		nl
	),
	menu_key(Key).
	
% Spracovanie klaves
menu_key(48) :- menu_exit. 		% 0
menu_key(49) :- menu_add_component.	% 1
menu_key(50) :- menu_remove_component. 	% 2
menu_key(51) :- menu_find_component. 	% 3
menu_key(52) :- menu_add_system. 	% 4
menu_key(53) :- menu_remove_system. 	% 5
menu_key(54) :- menu_find_system. 	% 6
menu_key(55) :- menu_load. 		% 7
menu_key(56) :- menu_save. 		% 8
menu_key(57) :- menu_list. 		% 9

% Pridanie komponentu
menu_add_component :-
	menu_input('Component name: ', Name), 
	(
		find_component(Name), 
		write('Component already exists.'), nl
	;
		menu_input('Component price: ', Price, number(Price)), 
		add_component(Name, Price),
		write('Component added.'), nl
	), 
	!, fail.
	
% Odobranie komponentu
menu_remove_component :-
	menu_input('Component name: ', Name), 
	(
		find_component(Name), 
		remove_component(Name),
		write('Component removed.'), nl
	;
		write('No such component.'), nl
	), 
	!, fail.
	
% Zobrazenie komponentu	
menu_find_component :-
	menu_input('Component name: ', Name), 
	(
		find_component(Name, Price), 
		write('Name: '), write(Name), nl, 
		write('Price: '), write(Price), nl
	;
		write('No such component.'), nl
	), 
	!, fail.
	
% Pridanie sytemu
menu_add_system :-
	menu_input('System name: ', Name), 
	(
		find_system(Name), 
		write('System already exists, adding to system.'), nl, 
		!, true
	;
		true
	), 
	(
		repeat, 
		menu_input('Component or subsystem name (end. when finished): ', Component, subsystem_predicate(Component)), 
		(
			Component \= 'end', 
			add_system(Name, Component),
			write('Component added to system.'), nl
		;
			Component = 'end'
		),
		Component = 'end', 	
		write('System added.'), nl	
	), 
	!, fail.
	
%% System, komponent alebo end
subsystem_predicate(Component) :-
	find_system(Component, _).
subsystem_predicate(Component) :-
	find_component(Component, _).
subsystem_predicate('end').

% Odobranie systemu
menu_remove_system :-
	menu_input('System name: ', Name), 
	(
		find_system(Name), 
		remove_system(Name),
		write('System removed.'), nl
	;
		write('No such system.'), nl
	), 
	!, fail.
	
% Zobrazenie systemu
menu_find_system :-
	menu_input('System name: ', Name), 
	(
		find_system(Name), 
		
		findall(Component, find_system_component(Name, Component), Components), 
		nl, 
		print_components(Name, Components), 
		write('-------------------------------------------'), nl, 
		sum_of_components(Components, Sum), 
		write(Sum), write('€'), nl
	;
		write('No such system.'), nl
	), 
	!, fail.

%% print_components(System, +Components)
%% Vypise stromovu strukturu systemu aj s cenami komponent
print_components(_, []).
print_components(System, [H | T]) :-
	% Pri komponente vypise cenu, inak iba odsadi
	(
		find_component(H, Price), 
		write(Price), write('€\t'), 
		!, true
		;
		find_system(H), 
		write('\t'), 
		!, true
	),  
	% Odsadi o hlbku vnorenia 
	system_component_depth(System, H, Depth), 
	(
		between(2, Depth, _), write('  '), fail
		; 
		!, true
	), 
	% Nazov
	write(H),
	nl, 
	print_components(System, T).
	
%% sum_of_components(+Components, Sum)
%% Spocita cenu systemu (rekurzivne so vsetkymi podkomponentmi)
sum_of_components([], 0).
sum_of_components([H | T], N) :-
	find_component(H, Price), 
	sum_of_components(T, P), 
	N is P + Price, 
	!, true.
sum_of_components([_ | T], N) :-
	sum_of_components(T, N).

% Nacitanie
menu_load :-
	clear_database, 
	load_components, 
	load_systems, 
	write('Database loaded.'), nl,
	!, fail.
	
%% Vymaze databazu
clear_database :-
	(
		retract(component(_, _)), 
		fail
		;
		true
	), 
	(
		retract(system(_, _)), 
		fail
		;
		true
	).
	
load_components :-
	see('components.db'),
	repeat,
	read(Term),
	(
		Term = end_of_file, 
		seen, 
		!, true
	;
		Component =.. [component|Term],
		not(Component),
		asserta(Component),
		fail
	).
	
load_systems :- 
	see('systems.db'),
	repeat,
	read(Term),
	(
		Term = end_of_file, 
		seen, 
		!, true
	; 
		System =.. [system|Term],
		not(System),
		asserta(System),
		fail
	).
	
% Ulozenie
menu_save :-
	(
		tell('components.db'),
		component(Name, Price),
		writeq([Name, Price]),
		write('.'),nl,
		%retract(component(Name, Price)),
		fail
		;
		told
	), 
	(
		tell('systems.db'),
		system(Name, Component),
		writeq([Name, Component]),
		write('.'),nl,
		%retract(system(Name, Component)),
		fail
		;
		told
	), 
	write('Database saved.'), nl,
	!, fail.
	
% Vypis databazy
menu_list :-
	write('Components: '), nl, nl,
	(
		find_component(Component, Price),
		write(Price), write('€\t'), write(Component), nl, 
		fail
		;
		true
	), 
	nl, write('Systems: '), nl, nl,
	(
		find_system(Name, Component),
		write(Name), write(' -> '), write(Component), nl, 
		fail
		;
		true
	), 
	!, fail.

% Koniec
menu_exit.

% Vstup od pouzivatela

%% Otazka a odpoved
menu_input(Question, Reply) :- 
	write(Question), 
	read(Reply).

%% Overenie platnosti predikatu (kontrola zadanych udajov)
menu_input(Question, Reply, Predicate) :- 
	write(Question), 
	read(Reply), 
	Predicate, 
	!.	
menu_input(Message, Value, Predicate) :- 
	write('Incorrect format.'), nl, 
	menu_input(Message, Value, Predicate).

%
% Praca s databazou
%

% Komponenty

:- dynamic component/2.

%component(processor, 40).
%component(memory, 30).
%component(graphics_card, 70).
%component(mother_board, 30).
%component(disk, 60).
%component(power_supply, 20).
%component(case, 40).
%component(monitor, 90).
%component(keyboard, 20).
%component(mouse, 15).

%% Pridanie komponentu
add_component(Name, Price) :- 
	asserta(component(Name, Price)).
	
%% Odstranenie komponentu s danym menom
remove_component(Name) :- 
	retract(component(Name, _)), 
	retract(system(_, Name)).

%% Najdenie komponentu s danym menom
find_component(Name) :-
	component(Name, _).
	
%% Najdenie komponentu s danym menom a cenou	
find_component(Name, Price) :-
	component(Name, Price).
	
% Systemy

:- dynamic system/2.

%system(mother_board, processor).
%system(mother_board, memory).
%system(mother_board, graphics_card).
%system(case, disk).
%system(case, power_supply).
%system(case, mother_board).
%system(computer, case).
%system(computer, monitor).
%system(computer, keyboard).
%system(computer, mouse).
	
%% Pridanie systemu
add_system(Name, Component) :- 
	not(system(Name, Component)), 
	asserta(system(Name, Component)).
	
%% Odstranenie systemu s danym menom
remove_system(Name) :- 
	(
		retract(system(Name, _)), 
		fail
		;
		true
	).

%% Najdenie systemu s danym menom
find_system(Name) :-
	system(Name, _).
	
%% Najdenie systemu s danym menom a podkomponentom alebo podsystemom
find_system(Name, Component) :-
	system(Name, Component).
	
%% find_system_component(System, Component)
%% Najdenie podkomponentov a podsystemov daneho systemu
find_system_component(Component, Component) :-
	component(Component, _).
find_system_component(Name, Component) :-
	system(Name, Component), 
	not(component(Component, _)).
find_system_component(Name, Component) :-
	system(Name, X), 
	find_system_component(X, Component).
	
%% system_component_depth(System, Component, Depth)
%% Zistenie hlbky daneho podkomponentu alebo podsystemu od daneho systemu
system_component_depth(Component, Component, 0).
system_component_depth(Name, Component, Depth) :-
	system(Name, X), 
	system_component_depth(X, Component, D), 
	Depth is D + 1.
	

