import librosa
import numpy as np
from pydub import AudioSegment

def pcm_to_mp3(pcm_file, mp3_file, sample_rate=44100, channels=1):
    """
    Обратная конвертация PCM в MP3
    """
    # Чтение PCM файла
    pcm_data = np.fromfile(pcm_file, dtype=np.int16)
    
    # Конвертация обратно в float (-1.0 до 1.0)
    # y = pcm_data.astype(np.float32)/32767.0
    
    audio = AudioSegment(
        data=pcm_data.tobytes(),
        sample_width=2,  # 16-bit = 2 байта
        frame_rate=sample_rate,
        channels=channels
    )
    
    # Экспорт в MP3
    audio.export(mp3_file, format="mp3", bitrate="192k")
    print(f"Конвертация завершена: {pcm_file} -> {mp3_file}")

def mp3_to_pcm(mp3_file,pcm_file):

    y, sr = librosa.load(mp3_file, sr=44100, mono=True)
    pcm_data = (y * 32767).astype(np.int16)
    pcm_data.tofile(pcm_file)



# mp3_to_pcm("resurse/in/ANNA_ASTI.mp3", "resurse/in/ANNA_ASTI.pcm")
# mp3_to_pcm("resurse/in/Smeshariki.mp3", "resurse/out/Smeshariki.pcm")
# pcm_to_mp3("resurse/out/rxdata_etalon.pcm", "resurse/out/smeshrestored.mp3")
pcm_to_mp3("resurse/out/rxdata_audio_2.pcm", "resurse/out/smeshrestored.mp3")