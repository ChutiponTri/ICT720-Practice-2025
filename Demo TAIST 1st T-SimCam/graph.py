import numpy as np
import matplotlib.pyplot as plt

def theoretical_rssi(rssi_0, n, distances):
    return rssi_0 - 10 * n * np.log10(distances)

# Example collected RSSI data (replace with actual values)
collected_rssi = {
    1: -50,   # RSSI at 1m
    2: -55,   # RSSI at 2m
    3: -60,   # RSSI at 3m
    4: -65,   # RSSI at 4m
    5: -70    # RSSI at 5m
}

distances = np.array(list(collected_rssi.keys()))
collected_values = np.array(list(collected_rssi.values()))

# Parameters for theoretical model
rssi_0 = -50  # Reference RSSI at 1 meter
n = 2.5  # Path loss exponent

# Generate smooth theoretical curve
theoretical_distances = np.linspace(1, 5, 100)
theoretical_values = theoretical_rssi(rssi_0, n, theoretical_distances)

# Plot the comparison
plt.figure(figsize=(8, 5))
plt.plot(theoretical_distances, theoretical_values, 'r-', label="Theoretical RSSI")  # Smooth curve
plt.scatter(distances, collected_values, color='b', label="Collected RSSI")  # 5 collected points
plt.xlabel("Distance (m)")
plt.ylabel("RSSI (dBm)")
plt.title("BLE RSSI Comparison")
plt.legend()
plt.grid()
plt.show()
