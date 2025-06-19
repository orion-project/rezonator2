#!/usr/bin/env python
# Example Python function for testing the FuncEditorWindow

import math

def calculate_beam_radius(wavelength, distance, waist_radius):
    """
    Calculate the beam radius at a given distance from the waist.
    
    Args:
        wavelength: Wavelength of the light in meters
        distance: Distance from the waist in meters
        waist_radius: Radius of the beam at the waist in meters
        
    Returns:
        Beam radius at the given distance in meters
    """
    z_r = math.pi * waist_radius**2 / wavelength  # Rayleigh range
    return waist_radius * math.sqrt(1 + (distance / z_r)**2)

# Example usage
wavelength = 1064e-9  # 1064 nm
waist_radius = 100e-6  # 100 µm
distances = [0, 0.1, 0.2, 0.3, 0.4, 0.5]  # meters

print("Beam radius calculation example:")
print("--------------------------------")
print(f"Wavelength: {wavelength*1e9:.1f} nm")
print(f"Waist radius: {waist_radius*1e6:.1f} µm")
print(f"Rayleigh range: {math.pi * waist_radius**2 / wavelength:.4f} m")
print("\nDistance (m) | Beam radius (µm)")
print("--------------|----------------")

for distance in distances:
    radius = calculate_beam_radius(wavelength, distance, waist_radius)
    print(f"{distance:12.2f} | {radius*1e6:14.2f}")
