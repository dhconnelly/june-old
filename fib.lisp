; assumes the following special forms:
; define, if, let, lambda

; assumes the following built-ins:
; nil nil? or < > + - cons car cdr display

(define (fib n)
    (if (< n 2)
        1
        (+ (fib (- n 1)) (fib (- n 2)))))

(define (range m n)
    (if (> m n)
        nil
        (cons m (range (+ m 1) n))))

(define (map f xs)
    (if (nil? xs)
        nil
        (let ((x (car xs)) (xs (cdr xs)))
            (cons (f x) (map f xs)))))

(define (zip xs ys)
    (if (or (nil? xs) (nil? ys))
        nil
        (let ((x (car xs))
              (xs (cdr xs))
              (y (car ys))
              (ys (cdr ys)))
            (cons '(x y) (zip (xs ys))))))

(define fibs
    (let ((double (lambda (x) (* 2 x)))
          (nums (map double (range -1 10))))
        (zip nums (map fib nums))))

(display "fibs: " fibs)
