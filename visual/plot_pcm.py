import numpy as np
import matplotlib.pyplot as plt
from icecream import ic

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

def rrc_filter(beta: float, span: int, sps: int) -> np.ndarray:
    """
    Создание импульсной характеристики Root Raised Cosine фильтра.

    beta : roll-off factor (0..1)
    span : длина фильтра в символах (обычно 6–10)
    sps  : количество сэмплов на символ (samples per symbol)
    """
    N = span * sps
    t = np.linspace(-span/2, span/2, N + 1)  # симметричная ось времени

    h = np.zeros_like(t)

    for i in range(len(t)):
        if t[i] == 0.0:
            h[i] = 1.0 - beta + (4 * beta / np.pi)
        elif abs(t[i]) == 1 / (4 * beta):
            h[i] = (beta / np.sqrt(2)) * (
                ((1 + 2/np.pi) * np.sin(np.pi/(4*beta))) +
                ((1 - 2/np.pi) * np.cos(np.pi/(4*beta)))
            )
        else:
            num = (
                np.sin(np.pi * t[i] * (1 - beta)) +
                4 * beta * t[i] * np.cos(np.pi * t[i] * (1 + beta))
            )
            den = np.pi * t[i] * (1 - (4 * beta * t[i])**2)
            h[i] = num / den

    # нормализуем энергию фильтра
    h = h / np.sqrt(np.sum(h**2))

    return h

def TED_loop_filter(data): #ted loop filter 
    BnTs = 0.01 
    Nsps = 10
    C = np.sqrt(2)
    Kp = 1
    teta = ((BnTs)/(Nsps))/(C + 1/(4*C))
    K1 = (-4*C*teta)/((1+2*C*teta+teta**2)*Kp)
    K2 = (-4*teta**2)/((1+2*C*teta+teta**2)*Kp)
    print("K1 = ", K1)
    print("K2 = ", K2)
    #K1_2 = (1/Kp)*((((4*C)/(Nsps**2))*((BnTs/(C + (1/4*C)))**2))/(1 + ((2 * C)/Nsps)*(BnTs/(C + (1/(4*C))))+(BnTs/(Nsps*(C+(1/4*C))))**2))
    err = np.zeros(len(data)//10)
    data = np.roll(data,-0)
    nsp = 10
    p1 = 0
    p2 = 0
    n = 0
    mass_cool_inex = []
    mass_id = []
    for ns in range(0,len(data)-(2*nsp),nsp):
        #real = (data.real[ns+n] - data.real[nsp+ns+n]) * data.real[n+(nsp)//2+ns]
        #imag = (data.imag[ns+n] - data.imag[nsp+ns+n]) * data.imag[n+(nsp)//2+ns]
        real = (data.real[nsp+ns+n] - data.real[ns+n]) * data.real[n + (nsp)//2+ns]
        imag = (data.imag[nsp+ns+n] - data.imag[ns+n] ) * data.imag[n + (nsp)//2+ns]
        err[ns//nsp] = real + imag
        # err[ns//nsp] = np.mean((np.conjugate(data[nsp+ns+n]) - np.conjugate(data[ns+n]))*(data[n + (nsp)//2+ns])) 
        # error = err.real[ns//nsp]
        error = err[ns//nsp]
        p1 = error * K1
        p2 = p2 + p1 + error * K2
        #print(ns ," p2 = ",p2)  
        while(p2 > 1):
            #print(ns ," p2 = ",p2)
            p2 = p2 - 1
        # while(p2 < -1):
        #     print(ns ," p2 = ",p2)
        #     p2 = p2 + 1
        
        n = round(p2*10)  
        # ic(ns, n, p2)
        n1 = n+ns+nsp   
        mass_cool_inex.append(n1)
        mass_id.append(n)

    #mass_cool_inex = [math.ceil(mass_cool_inex[i]) for i in range(len(mass_cool_inex))]
    mass1 = np.asarray(mass_cool_inex)
    mass = np.asarray(mass_id)
    plt.figure(figsize=(8, 8))
    plt.subplot(2,1,1)
    plt.title('TED')
    plt.plot(err)
    
    plt.xlabel("Samples")
    plt.ylabel("Error")
     
    plt.subplot(2,1,2)
    plt.plot(mass, 'o')  
    plt.title("Selected index")
    plt.xlabel("Samples")
    plt.ylabel("Index") 
    
    return mass1

def timing_recovery(IQ, n1st, Nsps, alg):

    BnTs = 0.01 
    Nsps = 10
    C = np.sqrt(2)
    Kp = 1
    teta = ((BnTs)/(Nsps))/(C + 1/(4*C))
    mi1 = (-4*C*teta)/((1+2*C*teta+teta**2)*Kp)
    mi2 = (-4*teta**2)/((1+2*C*teta+teta**2)*Kp)

    I = IQ.real
    Q = IQ.imag

    k = 0
    offs = 0
    adap1 = 0.0
    adap2 = 0.0

    ns = [n1st]
    partition = []
    error = []
    offset = []

    n = n1st
    while n < len(IQ) - 2 * Nsps:

        if alg == 1:
            # Gardner по I
            a = (I[n+Nsps+offs] - I[n+offs]) * I[n + Nsps//2 + offs]
            err = -a

        elif alg == 2:
            # Gardner по Q
            a = (Q[n+Nsps+offs] - Q[n+offs]) * Q[n + Nsps//2 + offs]
            err = -a

        elif alg == 3:
            # Gardner по I и Q
            a = (I[n+Nsps+offs] - I[n+offs]) * I[n + Nsps//2 + offs]
            b = (Q[n+Nsps+offs] - Q[n+offs]) * Q[n + Nsps//2 + offs]
            err = a + b    

        elif alg == 4:
            # Gardner комплексный, нечувствительный к freq offset
            err = -np.real(
                (np.conj(IQ[n+Nsps+offs]) - np.conj(IQ[n+offs])) *
                IQ[n + Nsps//2 + offs]
            )

        elif alg == 5:
            # Mueller–Muller
            a = I[n+offs] * np.sign(I[n+Nsps+offs]) - I[n+Nsps+offs] * np.sign(I[n+offs])
            b = Q[n+offs] * np.sign(Q[n+Nsps+offs]) - Q[n+Nsps+offs] * np.sign(Q[n+offs])
            err = -(a + b)

        else:
            raise ValueError("Unknown algorithm number")

        adap2 = mi1 * err
        adap1 = adap1 + adap2 + mi2 * err

        while adap1 > 1:
            adap1 -= 1
        while adap1 < -1:
            adap1 += 1

        offs = int(round(adap1 * Nsps))

        partition.append(adap1)
        error.append(err)
        offset.append(offs)

        k += 1
        ns.append(n + Nsps + offs)

        # шаг вперёд
        n += Nsps

    return ns

def FLL(conv):
    mu = 0
    omega = 0.8 # TODO: нужно протестировать для разных сигналов, пока непонятно, работает ли этот коэффициент для всех QPSK-сигналов
    freq_error = np.zeros(len(conv))
    output_signal = np.zeros(len(conv), dtype=np.complex128)

    for n in range(len(conv)):
        angle_diff = np.angle(conv[n]) - np.angle(output_signal[n-1]) if n > 0 else 0
        freq_error[n] = angle_diff / (2 * np.pi)
        omega = omega + mu * freq_error[n]
        output_signal[n] = conv[n] * np.exp(-1j * omega)
    return output_signal

N_fft = 128
CP = 32

# rx = read_bin_file("build/txdata2.pcm")
# rx = read_bin_file("/home/ivan/Desktop/SDR/SDR_club/resurse/out/txdata_qpsk.pcm")
rx = read_bin_file("/home/ivan/Desktop/SDR/SDR_club/resurse/out/rxdata_bpsk.pcm")
# rx = read_bin_file("/home/ivan/Desktop/SDR/SDR_club/resurse/out/2sdr.pcm")
# rx = rx[5000000:]
rx = rx
ic(len(rx))

# rrc = rrc_filter(0.25, 10, 10)
pulse = np.ones(10,'complex')
rx_filtered = np.convolve(rx, pulse, mode='full')
rx_filtered = rx_filtered/ np.max(rx_filtered)

sps = 10     

# after_filter = rx_filtered[10::sps][:len(rx)]

TED_index = TED_loop_filter(rx_filtered)
TED_index = TED_index
print(len(TED_index))
after_filter = rx_filtered[TED_index]    

# TED_index = timing_recovery(rx_filtered, 0, 10, 5)
# TED_index = TED_index
# print(len(TED_index))
# after_filter = rx_filtered[TED_index]

# maxe = np.argmax(rx_filtered) % 10 # типа TED
# ic(maxe)
# after_filter = rx_filtered[maxe::10] # типа TED

# env = np.abs(rx_filtered)
# best_phase = np.argmax(env) % sps
# after_filter = rx_filtered[best_phase::sps]

after_filter = after_filter[2000:] # после настройки
# after_filter = FLL(after_filter)

plt.figure(figsize=(12,12))
plt.subplot(2,2,1)
plt.plot(np.real(after_filter), label='After matched filter (real)')
# plt.plot(np.imag(rx_filtered), label='После matched filter (imag)')
plt.title(f"rx_filtered")
plt.xlabel("Sample index")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(2,2,2)
plt.title(f"QAM after TED")
plt.scatter(after_filter.real, after_filter.imag, color='blue') 
# plt.ylim(8000, -8000)
# plt.plot(np.real(rx_filtered), label='После matched filter (real)')
# plt.plot(np.imag(rx_filtered), label='После matched filter (imag)')
plt.legend(); plt.grid()

plt.subplot(2,2,3)
plt.title(f"RX signal")
plt.plot(np.real(rx)) 
plt.plot(np.imag(rx))
plt.grid()

plt.subplot(2,2,4)
plt.title(f"QAM RX")
plt.scatter(rx.real,rx.imag, color='blue') 
# plt.ylim(750, -750)
plt.grid()

plt.show()

