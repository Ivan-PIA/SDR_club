import numpy as np
import matplotlib.pyplot as plt


def resource_grid(data, count_frames, len_frame, Nfft, cp, title = ""): 

    """
        Параметры:

        `data`        - данные для ресурсной сетки 
        `count_frame` - количество OFDM фреймов бля ресурсной сетки
        `len_frame`   - количество ofdm-символов в ofdm фрейме
        `Nftt`        - количество поднесущих
        `cp`          - защитный префикс

    """

    # cp = 10
    data1 = data[:(Nfft+cp)*len_frame*count_frames]
    # print(len(data1))
    half_nfft = Nfft//2

    # преобразуем в матрицу 
    data_freq = data1.reshape(len_frame*count_frames, (Nfft+cp))

    # обрезаем циклический префикс
    data1 = data_freq[:, cp:]
    # производим преобразование фурье и транспонируем матрицу для удобного вывода на карте
    data2 = np.fft.fft(data1).T
    # data2 = np.fft.fftshift(data2)
    # переставляем строки местами из-за не шифтнутых частот
    temp = np.copy(data2[0:half_nfft, :])
    data2[0:half_nfft, :] = data2[half_nfft:Nfft, :]
    data2[half_nfft:Nfft, :] = temp

    #plt.figure()
    plt.title(title)
    plt.imshow(abs(data2), cmap='jet',interpolation='nearest', aspect='auto') #abs(10*np.log10(data2))
    plt.xlabel('OFDM symbol')
    plt.ylabel('Subcarriers')
    plt.colorbar()#label='dB'
    # plt.show()

def read_bin_file(name):
    

    
    imag = []
    real = []
    count = []
    counter = 0
    with open(name, "rb") as f:
        index = 0
        while (byte := f.read(2)):
            if(index %2 == 0):
                real.append(int.from_bytes(byte, byteorder='little', signed=True))
                
                counter += 1
                count.append(counter)
            else:
                imag.append(int.from_bytes(byte, byteorder='little', signed=True))
                
            index += 1

    return np.vectorize(complex)(np.asarray(real), np.asarray(imag))

def read_bin_file_float(name):
    with open(name, "rb") as f:
        data = np.fromfile(f, dtype=np.float32)  

    if len(data) % 2 != 0:
        raise ValueError("Ошибка: Количество чисел в файле нечетное!")

    real = data[0::2]  
    imag = data[1::2] 

    return real + 1j * imag


N_fft = 128
CP = 32

# Инициализируем список для хранения данных
# rx = read_bin_file("build/txdata2.pcm")
rx = read_bin_file("file/out/rxdata_audio.pcm")
# rx = read_bin_file("build/Smeshariki.pcm")
# print(rx[39500:40000])
plt.figure()
# plt.stem(abs(rx))
plt.plot(np.real(rx)) 
plt.plot(np.imag(rx))
# count_ofdm= int(len(rx)//(N_fft+CP))
# plt.figure()
# resource_grid(rx, count_ofdm, 1, N_fft, CP, title=" SOAPY " )
# print(len(rx))

# # data = corr_pss_time(rx,N_fft)
# rx = int(len(rx)//(N_fft+CP))

# plt.figure()

# resource_grid(rx, count_ofdm, 1, N_fft, CP, title=" SOAPY " )


# fig, axs = plt.subplots(2, 1, layout='constrained')
# plt.figure(2)
# # axs[1].plot(count, np.abs(data),  color='grey')  # Используем scatter для диаграммы созвездия
# plt.plot(abs(rx))  # Используем scatter для диаграммы созвездия
# # plt.plot(count,(real), color='blue')  # Используем scatter для диаграммы созвездия

# plt.figure(3)
# plt.scatter(rx.real,rx.imag, color='blue') 
plt.show()

