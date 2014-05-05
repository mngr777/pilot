(use-modules (srfi srfi-1))

(define (process-network-message message)
  (let ((type (first message)))
    (cond ((equal? type 'om)
           ;; (om object-id)
           (let ((object-id (second message)))
             (send-object-model object-id))))))
