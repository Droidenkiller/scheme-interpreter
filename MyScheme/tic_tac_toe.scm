(define field00 0)
(define field01 0)
(define field02 0)
(define field10 0)
(define field11 0)
(define field12 0)
(define field20 0)
(define field21 0)
(define field22 0)

(define (is-valid-input input)
	(if (eq? input "1")
		#t
	;else
		(if (eq? input "2")
			#t
		;else
			(if (eq? input "3")
				#t
			;else
				#f
			)
		)
	)
)

(define (is-input-free row col)
	(if (eq? row 1)
		(if (eq? col 1)
			(eq? field00 0)
		;else
			(if (eq? col 2)
				(eq? field01 0)
			;else
				(eq? field02 0)
			)
		)
	;else
		(if (eq? row 2)
			(if (eq? col 1)
				(eq? field10 0)
			;else
				(if (eq? col 2)
					(eq? field11 0)
				;else
					(eq? field12 0)
				)
			)
		;else (row 3)
			(if (eq? col 1)
				(eq? field20 0)
			;else
				(if (eq? col 2)
					(eq? field21 0)
				;else
					(eq? field22 0)
				)
			)
		)
        )
)

(define (convert-input-to-number input)
	(if (eq? input "1")
		1
	;else
		(if (eq? input "2")
			2
		;else
			(if (eq? input "3")
				3
			;else
				#f
			)
		)
	)
)

(define (get-row-input)
	(display "Enter the row you want to place your symbol in: ")
	(define row (read-line))
	(if (is-valid-input row)
		(begin
			(display "Chosen row: ")
			(display row)
			(display "\n")
			(convert-input-to-number row)
		)
	;else
		(begin
			(display "Row enterd is not a valid row. Please enter a number in the range 1-3.\n")
			(get-row-input)
		)
	)
)

(define (get-col-input)
	(display "Enter the column you want to place your symbol in: ")
	(define col (read-line))
	(if (is-valid-input col)
		(begin
			(display "Chosen col: ")
			(display col)
			(display "\n")
			(convert-input-to-number col)
		)
	;else
		(begin
			(display "Column entered is not a valid column. Please enter a number in the range 1-3.\n")
			(get-row-input)
		)
	)
)

(define (print-separation-line)
	(display "-----\n")
)

(define (print-board-row field0 field1 field2)
	(if (eq? field0 0)
		(display " |")
	;else
		(if (eq? field0 1)
			(display "O|")
		;else
			(display "X|")
		)
	)
	(if (eq? field1 0)
		(display " |")
	;else
		(if (eq? field1 1)
			(display "O|")
		;else
			(display "X|")
		)
	)
	(if (eq? field2 0)
		(display " ")
	;else
		(if (eq? field2 1)
			(display "O")
		;else
			(display "X")
		)
	)
	(display "\n")
)

(define (print-board)
	(display "\n")
	(print-board-row field00 field01 field02)
	(print-separation-line)
	(print-board-row field10 field11 field12)
	(print-separation-line)
	(print-board-row field20 field21 field22)
)

(define (get-input)
	(define row (get-row-input))
	(define col (get-col-input))
	(if (is-input-free row col)
		(cons row col)
	;else
		(begin
			(display "Field is already taken. Please choose a different field.\n")
			(get-input)
		)
	)
)

(define (make-next-move player)
	(display "Player ")
	(display player)
	(display "'s turn.\n")
	(define row_col (get-input))
	(define row (car row_col))
	(define col (cdr row_col))
	(if (eq? row 1)
		(if (eq? col 1)
			(set! field00 player)
		;else
			(if (eq? col 2)
				(set! field01 player)
			;else
				(set! field02 player)
			)
		)
	;else
		(if (eq? row 2)
			(if (eq? col 1)
				(set! field10 player)
			;else
				(if (eq? col 2)
					(set! field11 player)
				;else
					(set! field12 player)
				)
			)
		;else (row 3)
			(if (eq? col 1)
				(set! field20 player)
			;else
				(if (eq? col 2)
					(set! field21 player)
				;else
					(set! field22 player)
				)
			)
		)
	)
)

(define (is-board-full)
	(if (> field00 0)
		(if (> field01 0)
			(if (> field02 0)
				(if (> field10 0)
					(if (> field11 0)
						(if (> field12 0)
							(if (> field20 0)
								(if (> field21 0)
									(if (> field22 0)
										#t
									;else
										#f
									)
								;else
									#f
								)
							;else
								#f
							)
						;else
							#f
						)
					;else
						#f
					)
				;else
					#f
				)
			;else
				#f
			)
		;else
			#f
		)
	;else
		#f
	)
)

(define (main player)
	(make-next-move player)
	(print-board)
	(define winState (check-win-state))
	(if (> winState 0)
		(if (eq? winState 1)
			(display "Player 1 won.\n")
		;else
			(display "Player 2 won.\n")
		)
	;else
		(if (eq? (is-board-full) #f)
			(main (if (eq? player 1) 2 1)) ; Call main recursively until a player has won.
		;else
			(display "Draw. Thanks for playing...\n")
		)
	)
)

(define (check-row-win field1 field2 field3)
	(if (eq? field1 field2)
		(if (eq? field2 field3)
			field1
		;else
			0
		)
	;else
		0
	)
)

(define (check-win-state)
	(define winner (check-row-win field00 field01 field02))
	(if (> winner 0)
		winner
	;else
		(begin
			(set! winner (check-row-win field10 field11 field12))
			(if (> winner 0)
				winner
			;else
				(begin
					(set! winner (check-row-win field20 field21 field22))
					(if (> winner 0)
						winner
					;else
						(begin
							(set! winner (check-row-win field00 field11 field22))
							(if (> winner 0)
								winner
							;else
								(begin
									(set! winner (check-row-win field02 field11 field20))
									(if (> winner 0)
										winner
									;else
										(begin
											(set! winner (check-row-win field00 field10 field20))
											(if (> winner 0)
												winner
											;else
												(begin
													(set! winner (check-row-win field01 field11 field21))
													(if (> winner 0)
														winner
													;else
														(begin
															(set! winner (check-row-win field02 field12 field22))
															winner
														)
													)
												)
											)
										)
									)
								)
							)
						)
					)
				)
			)
		)
	)
)

(display "Started tic tac toe...\n")
(display "Note: Row and column indexes start at 1 and from the top left.\n")
(print-board)
(main 1)