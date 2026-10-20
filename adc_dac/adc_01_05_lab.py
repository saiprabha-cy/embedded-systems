
import numpy as np
import matplotlib.pyplot as plt

# ============================================================
# ADC-01: Voltage-to-code conversion
# ADC-02: Quantization experiment
# ADC-03: Resolution comparison
# ADC-04: Sampling-rate experiment
# ADC-05: DAC waveform generation and reconstruction
# ============================================================

VREF = 3.3
VMIN = 0.0


def adc_convert(voltage, bits, vref=VREF, vmin=VMIN):
    """Ideal unipolar ADC with floor-based quantization."""
    if bits < 1 or vref <= vmin:
        raise ValueError("Invalid resolution or reference range")

    levels = 2**bits
    x = np.asarray(voltage, dtype=float)
    x_clip = np.clip(x, vmin, vref)

    code = np.floor(
        (x_clip - vmin) / (vref - vmin) * levels
    ).astype(int)

    return np.clip(code, 0, levels - 1)


def adc_reconstruct(code, bits, vref=VREF, vmin=VMIN):
    """Reconstruct the centre of the selected quantization bin."""
    levels = 2**bits
    step = (vref - vmin) / levels
    return vmin + (np.asarray(code) + 0.5) * step


def dac_convert(code, bits, vref=VREF):
    """Ideal unipolar DAC; maximum code maps to VREF."""
    if bits < 1:
        raise ValueError("bits must be >= 1")

    max_code = 2**bits - 1
    code = np.clip(np.asarray(code), 0, max_code)
    return vref * code / max_code


# ------------------------------------------------------------
# ADC-01: Single voltage conversion
# ------------------------------------------------------------
vin = 1.65
code = int(adc_convert(vin, bits=12))

print("\nADC-01: CONVERSION")
print(f"Input voltage = {vin:.4f} V")
print(f"12-bit ADC code = {code}")
print(f"Estimated bin centre = "
      f"{float(adc_reconstruct(code, 12)):.6f} V")


# ------------------------------------------------------------
# ADC-02: Quantization error and staircase
# ------------------------------------------------------------
t = np.linspace(0, 1, 4000)
analog = 1.65 + 1.4 * np.sin(2 * np.pi * 3 * t)
analog = np.clip(analog, VMIN, VREF)

bits = 8
codes = adc_convert(analog, bits)
reconstructed = adc_reconstruct(codes, bits)
error = analog - reconstructed

step = (VREF - VMIN) / (2**bits)

print("\nADC-02: QUANTIZATION")
print(f"Resolution = {bits} bits")
print(f"LSB size = {step * 1000:.4f} mV")
print(f"Maximum absolute error in this sample = "
      f"{np.max(np.abs(error)) * 1000:.4f} mV")
print(f"Unsaturated ideal half-step bound = "
      f"{step * 500:.4f} mV")

plt.figure()
plt.plot(t, analog, label="Analog input")
plt.step(t, reconstructed, where="mid",
         label="Quantized reconstruction")
plt.xlabel("Time (s)")
plt.ylabel("Voltage (V)")
plt.title("ADC-02: Quantization Staircase")
plt.grid(True)
plt.legend()
plt.tight_layout()


plt.figure()
plt.plot(t, error * 1000)
plt.xlabel("Time (s)")
plt.ylabel("Quantization error (mV)")
plt.title("ADC-02: Quantization Error")
plt.grid(True)
plt.tight_layout()


# ------------------------------------------------------------
# ADC-03: Resolution comparison
# ------------------------------------------------------------
resolutions = [8, 10, 12, 16]

print("\nADC-03: RESOLUTION COMPARISON")
for nbits in resolutions:
    lsb = (VREF - VMIN) / (2**nbits)
    print(
        f"{nbits:2d}-bit: levels={2**nbits:6d}, "
        f"LSB={lsb * 1000:.6f} mV, "
        f"half-step={lsb * 500:.6f} mV"
    )

plt.figure()
for nbits in resolutions:
    sample_v = np.linspace(0, VREF, 1000)
    sample_code = adc_convert(sample_v, nbits)
    sample_rec = adc_reconstruct(sample_code, nbits)
    plt.plot(sample_v, sample_rec, label=f"{nbits}-bit")

plt.xlabel("Input voltage (V)")
plt.ylabel("Reconstructed voltage (V)")
plt.title("ADC-03: Resolution Comparison")
plt.grid(True)
plt.legend()
plt.tight_layout()


# ------------------------------------------------------------
# ADC-04: Sampling-rate and aliasing experiment
# ------------------------------------------------------------
signal_frequency = 50.0
duration = 0.2

t_fine = np.linspace(0, duration, 10000)
x_fine = np.sin(2 * np.pi * signal_frequency * t_fine)

sampling_rates = [30, 80, 200]

plt.figure()
plt.plot(t_fine, x_fine, label="Original 50 Hz signal")

for fs in sampling_rates:
    ts = np.arange(0, duration, 1 / fs)
    xs = np.sin(2 * np.pi * signal_frequency * ts)
    plt.plot(ts, xs, "o-", label=f"Sampled at {fs} Hz")

plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.title("ADC-04: Sampling Rate and Aliasing")
plt.grid(True)
plt.legend()
plt.tight_layout()

print("\nADC-04: SAMPLING")
for fs in sampling_rates:
    print(
        f"fs={fs:3d} Hz, Nyquist={fs / 2:5.1f} Hz, "
        f"signal={signal_frequency:.1f} Hz, "
        f"Nyquist criterion satisfied={fs > 2 * signal_frequency}"
    )


# ------------------------------------------------------------
# ADC-05: DAC waveform and zero-order hold
# ------------------------------------------------------------
fs = 1000.0
duration = 0.1
tone_frequency = 50.0
dac_bits = 8

sample_times = np.arange(0, duration, 1 / fs)
sample_values = 1.65 + 1.4 * np.sin(
    2 * np.pi * tone_frequency * sample_times
)
sample_values = np.clip(sample_values, 0, VREF)

dac_codes = adc_convert(sample_values, dac_bits)
dac_levels = dac_convert(dac_codes, dac_bits)

# Zero-order hold: each DAC level persists until the next sample.
t_zoh = np.linspace(0, duration, 10000, endpoint=False)
indices = np.floor(t_zoh * fs).astype(int)
indices = np.clip(indices, 0, len(dac_levels) - 1)
zoh_output = dac_levels[indices]

# Demonstration-only low-pass reconstruction using a moving average.
# This is NOT a designed reconstruction filter.
window_length = 11
filtered_output = np.convolve(
    zoh_output,
    np.ones(window_length) / window_length,
    mode="same"
)

print("\nADC-05: DAC")
print(f"DAC resolution = {dac_bits} bits")
print(f"DAC sample rate = {fs:.1f} samples/s")
print(f"Signal frequency = {tone_frequency:.1f} Hz")
print(f"Number of generated samples = {len(dac_codes)}")

plt.figure()
plt.plot(t_zoh, 1.65 + 1.4 * np.sin(
    2 * np.pi * tone_frequency * t_zoh
), label="Reference sine")
plt.step(t_zoh, zoh_output, where="post",
         label="DAC zero-order hold")
plt.plot(t_zoh, filtered_output,
         label="Moving-average demonstration")
plt.xlabel("Time (s)")
plt.ylabel("Voltage (V)")
plt.title("ADC-05: DAC Waveform Generation")
plt.grid(True)
plt.legend()
plt.tight_layout()

plt.show()
