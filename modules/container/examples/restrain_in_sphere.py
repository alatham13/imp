## \example container/restrain_in_sphere.py
# This fragment shows how to restrain a set of points stored in a
# SingletonContainer in a sphere of radius 'radius' centered around
# 'center'.

import IMP.example
import sys

IMP.setup_from_argv(sys.argv, "restrain in sphere")

radius = 10
stiffness = 2
center = IMP.algebra.Vector3D(1, 2, 3)
(m, c) = IMP.example.create_model_and_particles()
ub = IMP.core.HarmonicUpperBound(radius, stiffness)

# Restrain based on the distance to a single point (hence a ball)
ss = IMP.core.DistanceToSingletonScore(ub, center)

r = IMP.container.SingletonsRestraint(ss, c)
r.evaluate(False)
