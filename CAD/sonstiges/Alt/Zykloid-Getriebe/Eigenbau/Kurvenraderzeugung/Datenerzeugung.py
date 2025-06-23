## This code is to generate equation string as X and Y
## to use with Solidworks -> Equaiton Driven Curve
## to generate cycloidal disc sketch
'''
# ratio 14:1 with 7015GYEMS
R = 42
E = 1.5    #E < R/N
Rr = 4 
N = 15
'''

# ratio19:1 with 5010/5015 GYEMS servo
R = 28
E = 1      # E < R/N
Rr = 4
N = 20

# Definitions
# R : Radius of the roller's PCD (Pitch Circle Diameter)
# E : Eccentricity (or offset) from input shaft to a cycloid disc
# Rr : Radius of the roller
# N : Number of rollers
# Raio is N-1:1

# Precalculation
R_EN = R/(E*N)
NmE = N-E
_N = 1-N


#X = (R*cos(t))-(Rr*cos(t+arctan(sin((1-N)*t)/((R/(E*N))-cos((1-N)*t)))))-(E*cos(N*t))
#Y = (-R*sin(t))+(Rr*sin(t+arctan(sin((1-N)*t)/((R/(E*N))-cos((1-N)*t)))))+(E*sin(N*t))


X = "({}*cos(t)) - ( {}*cos( t+arctan( sin({}*t)/({} - cos({}*t)) ) ) ) - ({}*cos({}*t)) ".format(R,Rr,_N,R_EN,_N,E,N)
Y = "(-{}*sin(t)) + ( {}*sin( t+arctan( sin({}*t)/({} - cos({}*t)) ) ) ) + ({}*sin({}*t)) ".format(R,Rr,_N,R_EN,_N,E,N)


print(X)
print(Y)
