(define nil '())
(define #void (display ""))   ; return value from display is void

(define not (lambda (bool)
    (if bool #f #t)))

(define < (lambda (a b)
    (> b a)))
(define <= (lambda (a b)
    (not (> a b))))
(define >= (lambda (a b)
    (not (> b a))))

(define equal? (lambda (a b)
    (if (eq? a b)
	#t
    ; else
	(if (string? a)
	    (if (string? b)
		(string-eq? a b)
	    ; else
		#f
	    )
	 ; else
	    (if (number? a)
		(if (number? b)
		    (= a b)
		; else
		    #f
		)
	    ; else
		(if (cons? a)
		    (if (cons? b)
			(if (equal? (car a) (car b))
			    (equal? (cdr a) (cdr b))
			; else
			    #f
			)
		    ;
			#f
		    )
		; else
		    #f
		)
	    )
	)
    )))

(define (length l)
    (if (eq? l '())
	0
	(+ 1 (length (cdr l)))))

(define plus1 (lambda (n) (+ n 1)))
(define minus1 (lambda (n) (- n 1)))

; append (a b c d) (e f g h)
;   append  (b c d) (e f g h)
;  cons( a , (append (b c d) (e f g h)))

(define (append list1 list2)
    (if (eq? list1 '())
	list2
    ; else
	(cons (car list1) (append (cdr list1) list2))))

; (a b c)
; append (reverse (b c) (a))
(define (reverse l)
    (if (eq? l '())
	'()
    ;
	(append (reverse (cdr l)) (list (car l)))))


; ><

(define fac (lambda (n)
   (if (> n 1)
      (* n (fac (- n 1)))
      1)
   )
)

(define sumTo (lambda (n)
   (if (> n 0)
      (+ n (sumTo (- n 1)))
      0)
   )
)

;;
;; make-adder = function(c) {
;;    return function(n) {
;;       return n+c;
;;    }
;; }

;; topEnvironment
;;  make-adder c   --- topEnv
;;
;; lambda n  --- topEnv

(define make-adder (lambda (c)
    (lambda (n) (+ n c))))

(define add10 (make-adder 10))
(define add100 (make-adder 100))

(display "add10(100) = ") (display (add10 100)) (display "\n")

(define for-loop (lambda (start stop fn)
    (if (> start stop)
       nil
    ; else
       (begin
	   (fn start)
	   (for-loop (+ 1 start) stop fn)
       )
    )
))

(for-loop 1 10 (lambda (n) (display n) (display "\n")))

(define sum-to (lambda (n)
    (define sum 0)
    (for-loop 1 n (lambda (i) (set! sum (+ sum i))))))

(define sum-to1 (lambda (n)
    (if (= n 0)
	0
	(+ n (sum-to1 (- n 1))))))

(define sum-to2 (lambda (n)
    (define helper (lambda (n accu)
	(if (= n 0)
	    accu
	    (helper (- n 1) (+ accu n)))))
    (helper n 0)))

;
; function count-to1(n)
;    if (n == 0)
;        return 0
;    else
;        return 1 + count-to(n-1)
;
(define count-to1 (lambda (n)
    (if (= n 0)
	0
	(+ 1 (count-to1 (- n 1))))))

;
; function count-to2(n)
;    function helper(n, accu)
;        if (n == 0)
;            return accu
;        else
;            return helper(n-1, accu+1)
;    return
;         helper(n, 0);
;
(define count-to2 (lambda (n)
    (define helper (lambda (n accu)
	(if (= n 0)
	    accu
	    (helper (- n 1) (+ accu 1)))))
    (helper n 0)))

(define (pp-indent indent)
    (if (= indent 0)
	nil ; nothing
    ; else
	(begin
	    (display " ")
	    (pp-indent (- indent 1)))))

(define (pp-arglist expr indent)
    (if (eq? expr '())
	nil ; nothing
    ; else
	(begin
	    (display " ")
	    (pp (car expr) indent)
	    (pp-arglist (cdr expr) indent))))

(define (pp-if expr indent)
    (display "(if ")
    (pp (car (cdr expr)) (+ indent 4))
    (display "\n")
    (pp-indent (+ indent 4))
    (pp (car (cdr (cdr expr))) (+ indent 4))
    (display ")")
)

(define (pp-cons expr indent)
    (if (eq? (car expr) 'lambda)
	(pp-lambda expr indent)
    ; else
	(if (eq? (car expr) 'if)
	    (pp-if expr indent)
	; else
	    (begin
		(display "(")
		(pp (car expr) indent)
		(if (= (length (cdr expr)) 1)
		    (begin
			(display " ")
			(pp (car (cdr expr)) indent)
			(display ")"))
		; else
		    (begin
			(display "\n")
			(pp-indent indent)
			(pp-arglist (cdr expr) (+ indent 4))
			(display ")")))
	    )
	)
    )
)

(define (pp expr indent)
    (if (cons? expr)
	(pp-cons expr indent)
    ; else
	(display expr)
    )
    #void
)

(define (test)
    (test-1)
    (test-2))

(define (test-1)
    (display "test-1\n"))
(define (test-2)
    (display "test-2\n"))

(define (test3)
    (begin
	(test-1)
	(test-2)))

(load "compiler.scm")

(display "done with init.scm\n")
