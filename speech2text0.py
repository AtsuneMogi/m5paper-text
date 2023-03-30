import openai
import serial
import sounddevice as sd
import sys
import wavio as wv
from scipy.io.wavfile import write


ser = serial.Serial(
    port="/dev/ttyACM0",
    baudrate=115200
)
openai.api_key = open("api-key.txt", 'r').read()[:-1]
# Sampling frequency
freq = 44100
# Recording duration
duration = 5


def main():
    ser.close()
    ser.open()
    while (1):
        print("\n\nSay something:")
        # Start recorder with the given values
        # of duration and sample frequency
        recording = sd.rec(
            int(duration * freq),
            samplerate=freq, channels=2
        )
        # Record audio for the given number of seconds
        sd.wait()
        # This will convert the NumPy array to an audio
        # file with the given sampling frequency
        write("recording0.wav", freq, recording)
        # Convert the NumPy array to audio file
        wv.write("recording1.wav", recording, freq, sampwidth=2)
        file = open("recording1.wav", "rb")
        text = openai.Audio.transcribe("whisper-1", file)["text"]
        print(text)
        if ("おしまい" in text):
            sys.exit(1)
        ser.write((text + "\n").encode())
        print("Sent with success!")


if __name__ == "__main__":
    main()

