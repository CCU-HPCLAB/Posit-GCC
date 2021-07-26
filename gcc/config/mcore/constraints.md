;; Constraint definitions for the Motorola MCore
;; Copyright (C) 2011-2019 Free Software Foundation, Inc.

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3, or (at your option)
;; any later version.

;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

;; Register constraints.
(define_register_constraint "a" "LRW_REGS"
  "@internal")

(define_register_constraint "b" "ONLYR1_REGS"
  "@internal")

(define_register_constraint "c" "C_REGS"
  "@internal")

(define_register_constraint "x" "ALL_REGS"
  "@internal")

;; Integer constraints.
(define_constraint "I"
  "An integer in the range 0 to 127."
  (and (match_code "const_int")
       (match_test "IN_RANGE (ival, 0, 127)")))

(define_constraint "J"
  "An integer in the range 1 to 32."
  (and (match_code "const_int")
       (match_test "IN_RANGE (ival, 1, 32)")))

(define_constraint "K"
  "A shift operand, an integer in the range 0 to 31."
  (and (match_code "const_int")
       (match_test "IN_RANGE (ival, 0, 31)")))

;; Other constraints.

