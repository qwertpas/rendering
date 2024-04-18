from PIL import Image
import os

# Path to the folder containing BMP files
current_file_path = os.path.abspath(__file__) + ''
directory_path = os.path.dirname(current_file_path)
folder_path = directory_path + '/output'

print(folder_path)

# List all BMP files in the folder
# bmp_files = [f for f in os.listdir(folder_path) if f.endswith('.bmp')]

bmp_files = sorted([f for f in os.listdir(folder_path) if f.endswith('.bmp')])


# Create a list to store the images
images = []
for file_name in bmp_files:
    file_path = os.path.join(folder_path, file_name)
    img = Image.open(file_path)
    images.append(img)
    print(file_name)

# Save the images as a GIF
gif_path = folder_path + "output.gif"
images[0].save(gif_path, save_all=True, append_images=images[1:], duration=100, loop=0)