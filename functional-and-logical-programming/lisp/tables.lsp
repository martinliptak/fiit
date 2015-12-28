;
; Základné operácie pre prácu s reprezentáciu tabuľky
;

;; m-element-key :: (.) -> string
; kluc prvku tabulky
(defun m-element-key (elem)
	(first elem))
	
;; m-element-value :: (.) -> string
; hodnota prvku tabulky
(defun m-element-value (elem)
	(rest elem))

;; m-table-empty :: -> table
; prazdna tabulka
(defun m-table-empty () 
	 '())
	 
;; m-table-readt :: table string -> string
; ziska hodnotu kluca
(defun m-table-readt (table key)
	(cond ((null table) nil)
	      ((eq (m-element-key (first table)) key) (m-element-value (first table)))
	      ((> (m-element-key (first table)) key) nil) ; netreba hľadať ďalej
	      (t (m-table-readt (rest table) key))))
	 
;; m-table-writet :: table (.) -> table
; zapise (kluc . hodnota) do tabulky
(defun m-table-writet (table elem)
	(cond ((null table) (list elem))
	      ((< (m-element-key elem) (m-element-key (first table))) (cons elem table))
	      (t (cons (first table) (m-table-writet (rest table) elem)))))
	    
;; m-table-delete :: table string -> table 
; odstrani prvok z tabulky podla kluca
(defun m-table-delete (table key)
	(cond ((null table) nil)
	      ((eq (m-element-key (first table)) key) (rest table))
	      ((> (m-element-key (first table)) key) table) ; netreba hľadať ďalej
	      (t (cons (first table) (m-table-delete (rest table) key)))))
	 
;; m-table-isin :: table string -> bool
; nachadza sa kluc v tabulke?
(defun m-table-isin (table key)
	(not (eq (m-table-readt table key) nil))) ; ak m-table-readt niečo nájde
	
;; m-table-dom :: table -> []
; zoznam klucov tabulky
(defun m-table-dom (table)
	(cond ((null table) nil)
	      (t (cons (m-element-key (first table)) (m-table-dom (rest table))))))
	      
;; m-table-show :: table -> ignore
; vypise tabulku
; pouziva standardny vystup!
(defun m-table-show (table)
	(cond ((null table) nil)
	      (t (princ (m-element-key (first table)))
	         (princ " => ") 
	         (princ (m-element-value (first table)))
	         (terpri)
	         (m-table-show (rest table)))))
	      	      

