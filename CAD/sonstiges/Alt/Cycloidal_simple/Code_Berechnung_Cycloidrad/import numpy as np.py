import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backend_bases import MouseButton
from shapely.geometry.polygon import LinearRing

# Parameter
N = 15
R = 50
c = 0.5
pin_radius = 10  # Radius = Durchmesser/2

# Zykloiden-Punkte berechnen
theta = np.linspace(0, 2*np.pi, 1000)
x = R * np.cos(theta) + (R/N - c) * np.cos(N*theta)
y = R * np.sin(theta) + (R/N - c) * np.sin(N*theta)
cycloid_points = np.vstack([x, y]).T

# Offset für die Pins (Versatz um Pin-Radius)
offset_cycloid = LinearRing(cycloid_points).parallel_offset(pin_radius, "left").coords

# Plot
fig, ax = plt.subplots(figsize=(10, 10))
ax.set_aspect("equal")
ax.plot(x, y, color="blue", label="Zykloidenbahn")
ax.plot(*offset_cycloid.xy, color="red", linestyle="--", label="Offset (Pin-Kontur)")

# Pins zeichnen
for angle in np.linspace(0, 2*np.pi, N, endpoint=False):
    pin_x = R * np.cos(angle)
    pin_y = R * np.sin(angle)
    ax.add_patch(plt.Circle((pin_x, pin_y), pin_radius, fill=True, color="black"))

plt.legend()
plt.show()