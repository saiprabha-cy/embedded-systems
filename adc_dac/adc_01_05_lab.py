import numpy as np
import matplotlib.pyplot as plt

def adc_convert(voltage, bits, vref=3.3, vmin=0.0):
    """Ideal unipolar/general-offset ADC code model."""
    if bits < 1:
        raise ValueError("bits must be >= 1")
    if vref <= vmin:
        raise ValueError("vref must be greater than vmin")

    levels = 2**bits
    voltage = np.asarray(voltage, dtype=float)
    clipped = np.clip(voltage, vmin, vref)

    code = np.floor(
        (clipped - vmin) / (vref - vmin) * levels
    ).astype(int)

    return np.clip(code, 0, levels - 1)

vin = 1.65
code = int(adc_convert(vin, bits=12, vref=3.3))
print("Input voltage:", vin, "V")
print("ADC code:", code)