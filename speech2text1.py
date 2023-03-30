import serial
import sounddevice as sd
import sys
import wavio as wv
import whisper
from scipy.io.wavfile import write


ser = serial.Serial(
    port="/dev/ttyACM0",
    baudrate=115200
)

# Sampling frequency
freq = 44100
# Recording duration
duration = 5

model = whisper.load_model("medium")


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
        # load audio and pad/trim it to fit 30 seconds
        audio = whisper.load_audio("recording1.wav")
        audio = whisper.pad_or_trim(audio)
        # make log-Mel spectrogram and move to the same device as the model
        mel = whisper.log_mel_spectrogram(audio).to(model.device)
        # detect the spoken language
        _, probs = model.detect_language(mel)
        print(f"Detected language: {max(probs, key=probs.get)}")
        # decode the audio
        options = whisper.DecodingOptions()
        result = whisper.decode(model, mel, options)
        # recognized text
        text = result.text
        print(text)
        if ("おしまい" in text):
            sys.exit(1)
        ser.write((text + "\n").encode())
        print("Sent with success!")


if __name__ == "__main__":
    main()

