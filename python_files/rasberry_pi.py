import speech_recognition as sr
import RPi.GPIO as GPIO
from time import sleep
import smtplib
import os
import glob
import cv2
import serial
# Importing modules for sending mail
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders

GPIO.setmode(GPIO.BCM)
GPIO.setup(21, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(20, GPIO.IN, pull_up_down=GPIO.PUD_UP)

r = sr.Recognizer()
speech = sr.Microphone(device_index=2)

try:
    print("[INFO] Connecting To  Board")
    ser = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)     #enter ur arduino COM port number
    print("Sucessfully Connected")
except:
    print("[INFO] Failed To Connect To Arduino Check COM Port Number And Connection")
    pass


sender = 'k3595722@gmail.com'
password = 'thegreat1'
receiver = 'sv.apoorva39@gmail.com'

DIR = 'Images/'
FILE_PREFIX = 'image'

def word():
    with speech as source:
        print("say something!…")
        audio = r.adjust_for_ambient_noise(source)
        audio = r.listen(source)
    try:
        recog = r.recognize_google(audio, language = 'en-US')
        print("You said: " + recog)
        if recog == "help":
            print("sending sms")
            ser.write(b's')
    except sr.UnknownValueError:
        print("Google Speech Recognition could not understand audio")
    except sr.RequestError as e:
        print("Could not request results from Google Speech Recognition service; {0}".format(e))

def capture():
    print ('Sending E-Mail')
    # Create the directory if not exists
    if not os.path.exists(DIR):
        os.makedirs(DIR)
    files = sorted(glob.glob(os.path.join(DIR, FILE_PREFIX + '[0-9][0-9][0-9].jpg')))
    count = 0
   
    if len(files) > 0:
        # Grab the count from the last filename.
        count = int(files[-1][-7:-4])+1

    # Save image to file
    filename = os.path.join(DIR, FILE_PREFIX + '%03d.jpg' % count)
    # Capture the face
    cap = cv2.VideoCapture(0)
    ret,img = cap.read()
    cv2.imwrite(filename, img)
    # Sending mail
    msg = MIMEMultipart()
    msg['From'] = sender
    msg['To'] = receiver
    msg['Subject'] = 'Captured Photo'
   
    body = 'Picture is Attached.'
    msg.attach(MIMEText(body, 'plain'))
    attachment = open(filename, 'rb')
    part = MIMEBase('application', 'octet-stream')
    part.set_payload((attachment).read())
    encoders.encode_base64(part)
    part.add_header('Content-Disposition', 'attachment; filename= %s' % filename)
    msg.attach(part)
    server = smtplib.SMTP('smtp.gmail.com', 587)
    server.starttls()
    server.login(sender, password)
    text = msg.as_string()
    server.sendmail(sender, receiver, text)
    server.quit()
    print("Mail Sent")
   
while True:
    sleep(1)
    print("[INFO] Waiting for input")
    wordval = GPIO.input(20)
    camval = GPIO.input(21)
    if wordval == 0:
        word()
    if camval == 0:
	dfdf