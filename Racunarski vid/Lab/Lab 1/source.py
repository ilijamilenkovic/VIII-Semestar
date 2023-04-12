import cv2
import numpy as np
import matplotlib.pyplot as plt
def fft(img):
    img_fft = np.fft.fft2(img)  # pretvaranje slike u frekventni domen (FFT - Fast Fourier Transform), fft2 je jer je u 2 dimenzije
    img_fft = np.fft.fftshift(img_fft)  # pomeranje koordinatnog pocetka u centar slike
    return img_fft

def inverse_fft(magnitude_log, complex_moduo_1):
    img_fft = complex_moduo_1 * np.exp(magnitude_log) # vracamo magnitudu iz logaritma i mnozimo sa kompleksnim brojevima na slici
    img_filtered = np.abs(np.fft.ifft2(img_fft)) # funkcija ifft2 vraca sliku iz frekventnog u prostorni domen, nije potrebno raditi ifftshift jer to se implicitno izvrsava
                                                 # rezultat ifft2 je opet kompleksna slika, ali nas zanima samo moduo jer to je nasa slika zato opet treba np.abs()

    return img_filtered

def ukloni_piksel(img, diff_treshold):
    img_fft = fft(img)
    img_fft_mag = np.abs(img_fft)
    img_pha = img_fft / img_fft_mag
    matrix = np.log(img_fft_mag)
    plt.imshow(matrix)
    plt.show()
    x_max = img.shape[0] - 1 
    y_max = img.shape[1] - 1
    #print("x_max je " + str(x_max))
    for x in range(0, x_max):
        for y in range(0, y_max):
            mean = 0
            #1 x=0,y=0
            if x == 0 and y == 0:
                mean = (matrix[x+1][y] + matrix[x][y+1] + matrix[x+1][y+1])/3
            
            #2 x=0 y!=0 && y!=max
            elif x == 0 and y != 0 and y != y_max:
                mean = (matrix[x][y+1] + matrix[x+1][y+1] + matrix[x+1][y] + matrix[x+1][y-1] + matrix[x][y-1])/5
            #3 x=0, y=max
            elif x == 0 and y == y_max:
                mean = (matrix[x][y-1] + matrix[x+1][y] + matrix[x+1][y-1]) / 3
            #4 x!=0 and y = 0
            elif x != 0 and y == 0:
                mean = (matrix[x-1][y] + matrix[x-1][y+1] + matrix[x][y+1] + matrix[x+1][y+1] + matrix[x+1][y]) / 5
            #5 x!=0 and y = max
            elif x!=0 and y == y_max:
                mean = (matrix[x-1][y] + matrix[x-1][y-1] + matrix[x][y-1] + matrix[x+1][y-1] + matrix[x+1][y]) / 5
            #u sredinu
            else:
                 mean = (matrix[x][y-1] + matrix[x][y+1] + matrix[x-1][y] + matrix[x-1][y+1] + matrix[x-1][y-1] + matrix[x+1][y-1]  + matrix[x+1][y+1] + matrix[x+1][y])/8
            if matrix[x][y] - mean > diff_treshold:
                matrix[x][y] = mean
    plt.imshow(matrix)
    plt.show()
    img_filtered = inverse_fft(matrix, img_pha)
    return img_filtered



#Load gray image
img = cv2.imread('./slika_2.png')
img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
img_cista = ukloni_piksel(img, 3.04)


plt.imshow(img_cista, cmap="gray")
plt.show()



