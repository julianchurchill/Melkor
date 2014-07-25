#
# Eye Shape Patterns
# ------------------ 
#
# The syntax of this file is very similar to eyes.db from GNUGo3.4
# Used by LifeDeathGoal class.
#
# Pattern key:
#  
#  'O' = Stone of target string colour.
#  'X' = Stone of string enemy colour.
#  'o' = Stone of target string colour or empty point.
#  'x' = Stone of string enemy colour or empty point.
#  '.' = Empty point.
#  '|' = Vertical edge.
#  '-' = Horizontal edge.
#  '+' = A corner.
#  '?' = Don't care, i.e. any value.
#  '*' = Key point if present.
#
# Value after the colon indicates the following:
#   1. First value indicates how many eyes the string can get if the attacker moves first.
#   2. Second value indicates how many eyes the string can get if the defender moves first.
#
# After the two eye-values, the usable symmetries of the shape are listed, applied, in order of the list,
# to the original pattern. Possible values for symmetry are:
#  NONE  - No symmetry.
#  '|'   - Y axis reflection
#  '-'   - X axis reflection
#  '+'   - Y and X axis reflection (both '|' and '-')
#  '/'   - X=Y reflection
#  '\'   - X=-Y reflection
#  'X'   - Both X=Y and X=-Y reflection ('/' and '\')
# REFALL - All reflective symetries.
# ROT90  - 90 degree rotation   (equivalent to X=Y followed by X reflection)
# ROT180 - 180 degree rotation  (equivalent to X followed by Y reflection)
# ROT270 - 270 degree rotation  (equivalent to X=Y followed by Y reflection)
# ROTALL - 90, 180 and 270 degree rotation
#  ALL   - All symmetries reflective and rotational, 8 in total.
#
# There's no need to distinguish between vertical and horizontal edges, this
# is included here for readability considerations only. Name indicates the name
# of the pattern, typically a number the first value indicating the size of the eye space.


Name 101

OOO
OxO
OO?

:11,ROTALL


Name 102

OOO
OxO
---

:11,ROTALL


Name 103

|OO
|xO
+--

:11,ROTALL


Name 201

OOOO
OxxO
OOO?

:11,ALL


Name 301

OOOOO
O.*.O
OOOO?

:12,ALL



Name 501
# Five point nakade

OOOO
O..OO
O.*.O
OOOOO

:12,ALL