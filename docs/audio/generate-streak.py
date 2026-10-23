"""Generate Cubbl's original offline streak chime with Python's standard library."""
import math
from pathlib import Path
import struct
import wave

RATE = 22050
DURATION = 0.48
NOTES = [(0.0, 523.25), (0.075, 659.25), (0.15, 783.99), (0.225, 1046.50)]
samples = []
for i in range(round(RATE * DURATION)):
    t = i / RATE
    value = 0.0
    for start, frequency in NOTES:
        age = t - start
        length = DURATION - start
        if 0 <= age < length:
            envelope = min(1.0, age / 0.006) * math.exp(-12 * age)
            envelope *= min(1.0, (length - age) / 0.04)
            phase = 2 * math.pi * frequency * age
            value += envelope * (math.sin(phase) + 0.2 * math.sin(2 * phase))
    samples.append(value)
peak = max(abs(value) for value in samples)
output = Path(__file__).resolve().parents[2] / 'app/src/main/res/raw/streak.wav'
with wave.open(str(output), 'wb') as audio:
    audio.setparams((1, 2, RATE, 0, 'NONE', 'not compressed'))
    audio.writeframes(b''.join(struct.pack('<h', round(value / peak * 0.7 * 32767)) for value in samples))
print(output)
