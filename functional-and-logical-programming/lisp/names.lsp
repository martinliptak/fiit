;
; Praca s tabulkami podla mien
;

;; m-init
;; inicializacia, vytvori zoznam tabuliek
(defun m-init ()
	(setq tabulky '()))

;; m-table-name :: (.) -> string
;; vrati meno tabulky z prvku zoznamu tabuliek
(defun m-table-name (table)
	(first table))
	
;; m-table-value :: (.) -> table
;; vrati reprezentaciu tabulky z prvku zoznamu tabuliek
(defun m-table-value (table)
	(rest table))
	     
;; m-set-table :: string table -> ignore
;; nastavi meno na reprezentaciu tabulky
(defun m-set-table (name table)
	(setq tabulky (m-set-table-aux tabulky name table)))

(defun m-set-table-aux (list name table)
	(cond ((null list) (list (cons name table)))
	      ((eq name (m-table-name (first list))) (cons (cons (m-table-name (first list)) table) (rest list)))
	      (t (cons (first list) (m-set-table-aux (rest list) name table)))))

;; m-get-table :: name -> table
;; ziska reprezentaciu tabulky podla mena
(defun m-get-table (name)
	 (m-get-table-aux tabulky name))
	 	
(defun m-get-table-aux (list name)
	(cond ((null list) 'nothing)
	      ((eq name (m-table-name (first list))) (m-table-value (first list)))
	      (t (m-get-table-aux (rest list) name))))

;; m-empty :: string -> ignore
;; vytvori prazdnu tabulku s menom
(defun m-empty (name)
	(m-set-table name (m-table-empty)))

;; m-readt :: string string -> string
;; ziska hodnotu z tabulky s menom podla kluca
(defun m-readt (name key)
	(m-table-readt (m-get-table name) key))

;; m-writet :: string string -> ignore
;; nastavi hodnotu v tabulke s menom podla kluca
(defun m-writet (name elem)
	(m-set-table name (m-table-writet (m-get-table name) elem)))
	
;; m-delete :: string string -> string
;; vymaze zaznam z tabulky s menom podla kluca
(defun m-delete (name key)
	(m-set-table name (m-table-delete (m-get-table name) key)))
	
;; m-isin :: string string -> bool
;; otestuje pritomnost kluca v tabulke s menom
(defun m-isin (name key)
	(m-table-isin (m-get-table name) key))
	
;; m-dom :: string -> []
;; vrati zoznam klucov tabulky s menom
(defun m-dom (name)
	(m-table-dom (m-get-table name)))

;; m-show :: string -> ignore
;; vypise zaznamy tabulky s menom
;; pouziva standardny vystup!
(defun m-show (name)
	(m-table-show (m-get-table name)))
	
;; m-show
;; vypise zaznamy vsetkych tabuliek
;; pouziva standardny vystup!
(defun m-show-all ()
	(m-show-all-aux tabulky))

(defun m-show-all-aux (list)
	(cond ((null list) nil)
	      (t (print (m-table-name (first list)))
	         (terpri)
	         (m-table-show (m-table-value (first list)))
	         (terpri)
	         (m-show-all-aux (rest list)))))
	
