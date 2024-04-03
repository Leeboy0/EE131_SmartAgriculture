import requests
import serial
import time

def get_la_weather(api_key):
    url = "http://api.openweathermap.org/data/2.5/weather"
    params = {
        'q': 'Los Angeles,US',
        'appid': api_key,
        'units': 'metric'  # or 'imperial' for Fahrenheit
    }

    response = requests.get(url, params=params)
    if response.status_code == 200:
        data = response.json()
        temperature = data['main']['temp']
        print(f"Temperature in Los Angeles: {temperature}°C")
        return temperature
    else:
        print("Error fetching weather data")
        return None

api_key = 'e84e39326749b92dedb3066415a6a317'
temperature = get_la_weather(api_key)

if temperature is not None:
    try:
        ser = serial.Serial('COM3', 9600, timeout=1)
        time.sleep(2)  # Wait for the serial connection to initialize

        ser.write(f"{temperature}\n".encode())

        ser.close()
    except Exception as e:
        print(f"Failed to send temperature over serial: {e}")


