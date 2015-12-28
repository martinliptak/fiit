;
; Používateľské rozhranie
;

;; help
; vypise prikazy
(defun help ()
	(princ "empty        -> create one empty evil table") (terpri)
  	(princ "readt        -> read key value from evil table") (terpri)
  	(princ "writet       -> write record to evil table") (terpri)
  	(princ "delete       -> delete record from evil table (use with caution)") (terpri)
  	(princ "isin         -> test presence of key in evil table") (terpri)
  	(princ "dom          -> list all keys in evil table") (terpri)
  	(princ "show         -> list all records in evil table") (terpri)
  	(princ "show-all     -> list all evil tables") (terpri) 
  	(princ "what?        -> show this messages") (terpri)
  	(princ "dismiss      -> dismiss me") (terpri) (terpri)
  	(princ "Now, it's time to enter your command, master. ") (terpri)
)

;; confirm :: string -> bool
; ziska (ne)suhlas od pouzivatela
(defun confirm (question)
	(princ question)
	(princ "(yes/anything else) ")
	(cond ((eq (read) 'yes) t)
	      (t nil))) 
	
;; ask :: string -> input
; ziska vstup od pouzivatela
(defun ask (question)
	(princ question)
	(read))

;; ui
; hlavna prog forma
(defun ui ()
	(prog (command name key)
		(terpri)
		(princ "Hello. ") 
		(princ "Welcome to evil-table-store idiot-proof interface (running on your personal lisp machine). I don't doubt, you already know all the commands. However, I'll list them for you, so that you don't have to waste time reckoning, sir.") (terpri) (terpri)
		(help)
		zac
		(princ "> ")
		(setq command (read))
		(cond ((eq command 'empty) 
				(setq name (ask "What shall the name be? "))
				(cond ((and (not (eq ( m-get-table name) 'nothing))
				            (not (confirm "Such table exists, sir. It'll be destroyed. Continue? ")))
						(princ "No table shall be overwritten.") (terpri))
				      (t 
				      		(m-empty name) 
				      		(princ "New empty evil table created.") (terpri))))
		      ((eq command 'readt) 
				(setq name (ask "What table shall be read from? "))
				(cond ((eq (m-get-table name) 'nothing)
					(princ "Whoaa! Table not found.") (terpri))
			      	      (t 
			      	      	(setq key (ask "What key shall be retrieved? "))
			      	      	(cond ((numberp key)
						(setq value (m-readt name key))
						(cond (value 
				      			(princ value) (terpri))
				      		      (t (princ "Sorry, sir, no such key in this table.") (terpri))
				      	      	))
				      	      (t (princ "Key must be a number.") (terpri))
				      	      	))))
		      ((eq command 'writet) 
				(setq name (ask "What table shall be written to? "))
				(cond ((eq (m-get-table name) 'nothing)
					(princ "Whoaa! Table not found.") (terpri))
			      	      (t 
					(setq key (ask "What key shall be used for storing? "))
					(cond ((numberp key)
							(setq value (ask "What value shall be written? "))
				      			(m-writet name (cons key value))
				      			(princ "Written to table.") (terpri))
				      	      (t (princ "Key must be a number.") (terpri))))))
		      ((eq command 'delete) 
				(setq name (ask "What table shall be deleted from? "))
				(cond ((eq (m-get-table name) 'nothing)
					(princ "Whoaa! Table not found.") (terpri))
			      	      (t 
			      	      	(setq key (ask "What key shall be deleted? "))
			      	      	(cond ((numberp key)
				      	      	(setq value (m-readt name key))
						(cond (value 
				      			(princ value) (terpri)
				      			(cond ((confirm "This can't be undone. Sure delete this record? ")
				      				(m-delete name key) (princ "Record deleted.") (terpri))
				      				(t (princ "Nothing deleted.") (terpri))))
				      		      (t (princ "Sorry, sir, no such key in this table.") (terpri))))
				      		(t (princ "Key must be a number.") (terpri))))))
		      ((eq command 'isin) 
				(setq name (ask "What table shall be read from? "))
				(cond ((eq (m-get-table name) 'nothing)
					(princ "Whoaa! Table not found.") (terpri))
			      	      (t 
			      	      	(setq key (ask "What key shall be retrieved? "))
			      	      	(cond ((numberp key)
						(setq value (m-readt name key))
						(cond (value 
				      			(princ "Yes, it seems that such key exists, sir.") (terpri))
				      		      (t (princ "Nope.") (terpri))
				      	      	))
				      	       (t (princ "Key must be a number.") (terpri))))))
		      ((eq command 'dom) 
				(setq name (ask "What table shall be shown? "))
				(cond ((eq (m-get-table name) 'nothing)
					(princ "Whoaa! Table not found.") (terpri))
			      	      (t 
			      	      	(princ "Showing the keys.")
			      	      	(print (m-dom name)) (terpri))))
		      ((eq command 'show) 
				(setq name (ask "What table shall be shown? "))
				(cond ((eq (m-get-table name) 'nothing)
					(princ "Whoaa! Table not found.") (terpri))
			      	      (t 
			      	      	(princ "Showing the table.") (terpri)
			      	      	(m-show name))))
		      ((eq command 'show-all) 
		      		(princ "Your overwhelming genius wants to see all tables to plan the best strategy for world domination. Here they are, master.") (terpri) 
		      		(m-show-all))
		      ((eq command 'what?) 
		      		(princ "At your service.") (terpri) (terpri) (help))
		      ((eq command 'dismiss)
		      		(princ "Good luck devising your evil plans to rule the world, master. Good bye.") (terpri) 
		      		(return))
		      (t 
		      		(princ "I don't understand, sir.") (terpri) (terpri) (help))
		)
		(go zac)
	)
)
