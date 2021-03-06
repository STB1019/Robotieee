(define (domain sokobanSequential)
  (:requirements :typing :action-costs)
  (:types thing location direction - object
          player stone - thing)
  (:predicates (clear ?l - location)
	       (at ?t - thing ?l - location)
	       (at-goal ?s - stone)
	       (IS-GOAL ?l - location)
	       (IS-NONGOAL ?l - location)
               (MOVE-DIR ?from ?to - location ?dir - direction))
 ;; (:functions (total-cost) - number)

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
                      )
   )

  (:action pull-to-nongoal
   :parameters (?p - player ?s - stone
                ?p_pos ?from ?to - location
                ?dir - direction)
   :precondition (and (at ?p ?p_pos)
                      (at ?s ?from)
                      (clear ?to)
                      (MOVE-DIR ?p_pos ?to ?dir)
                      (MOVE-DIR ?from ?p_pos ?dir)
                      (IS-NONGOAL ?p_pos)
                      )
   :effect       (and (not (at ?p ?p_pos))
                      (not (at ?s ?from))
                      (not (clear ?to))
                      (at ?p ?to)
                      (at ?s ?p_pos)
                      (clear ?from)
                      (not (at-goal ?s))
                 )
   )

  (:action pull-to-goal
   :parameters (?p - player ?s - stone
                ?p_pos ?from ?to - location
                ?dir - direction)
   :precondition (and (at ?p ?p_pos)
                      (at ?s ?from)
                      (clear ?to)
                      (MOVE-DIR ?p_pos ?to ?dir)
                      (MOVE-DIR ?from ?p_pos ?dir)
                      (IS-GOAL ?p_pos)
                      )
   :effect       (and (not (at ?p ?p_pos))
                      (not (at ?s ?from))
                      (not (clear ?to))
                      (at ?p ?to)
                      (at ?s ?p_pos)
                      (clear ?from)
                      (at-goal ?s)
                 )
   )
)
