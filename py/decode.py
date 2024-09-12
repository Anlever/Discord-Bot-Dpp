import sys
import base64
import os
from datetime import datetime


file_path = sys.argv[1]


with open(file_path, 'r') as file:
    base64_data = file.read().strip()

current_time = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
file_name = f"image_{current_time}.png"
directory = r"C:\Users\Anlever1\Desktop\Discord Bot C++\tempImg"

os.makedirs(directory, exist_ok=True)

file_path = os.path.join(directory, file_name)

with open(file_path, 'wb') as f:
    f.write(base64.b64decode(base64_data))

print(file_path)
