(define (domain exploration)
  (:requirements :typing :action-costs)
  (:types thing location direction - object
          player - thing)
  (:predicates 
               (clear ?l - location)
               (visited ?l - location)
	       (at ?t - thing ?l - location)
               (MOVE-DIR ?from ?to - location ?dir - direction)
  )
  (:action move
   :parameters (?p - player ?from ?to - location ?dir - direction)
   :precondition (and (at ?p ?from)
                      (clear ?to)
                      (MOVE-DIR ?from ?to ?dir)
                 )
   :effect       (and (not (at ?p ?from))
                      (not (clear ?to))
                      (at ?p ?to)
                      (clear ?from)
                      (visited ?to)
                 )
  )
)
