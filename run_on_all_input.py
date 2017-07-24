import os
import shlex
import shutil
import sys

#USAGE: 
# - python3 run_on_all_input.py <PATH_TO_CONVERTER_PROGRAM> <PATH_TO_IMAGE_WRITER_PROGRAM> [arm]
#
# This will create a folder called output with all of the
# output RGB files (after converting to YCbCr and then back to RGB)
# as well as the corresponding image files in PNG format. If you include the
# 'arm' argument, then it will run the C executable on the ARM simulator
# (assuming the program was compiled for ARM)

if len(sys.argv) <= 2:
	print("You must provide the location of a converter program and image writer program")
	sys.exit()

root_dir = os.path.dirname(os.path.realpath(__file__))
input_data_dir = os.path.join(root_dir, "data/RGB")
output_data_dir = os.path.join(root_dir, "output")
converter_program_location = os.path.abspath(sys.argv[1])
image_writer_program_location = os.path.abspath(sys.argv[2])


files = {}

# Delete the output folder and then recreate it
if os.path.exists(output_data_dir):
	shutil.rmtree(output_data_dir)
os.makedirs(output_data_dir)

# Get the file paths to all of the test data
for file in os.listdir(input_data_dir):
    if file.endswith(".txt"):
    	files[file] = os.path.join(input_data_dir, file)


# Run the C Program on all test inputs.
# Then convert the resulting C program outputs to image files using MATLAB
for file, filepath in files.items():
	output_file = os.path.join(output_data_dir, file)
	output_image = output_file.replace(".txt", ".png")

	if len(sys.argv) >= 4 and sys.argv[3] == "arm":
		print("Running colour converter c program on ARM processor on file '{0}'".format(filepath))
		if os.system("qemu-arm {0} < {1} > {2}".format(shlex.quote(converter_program_location), shlex.quote(filepath), shlex.quote(output_file))) != 0:
			sys.exit()
		print("Creating file '{0}'".format(output_image))
		os.system("qemu-arm {0} {2} < {1}".format(shlex.quote(image_writer_program_location), shlex.quote(output_file), shlex.quote(output_image)))
	else:
		print("Running colour converter c program on  file '{0}'".format(filepath))
		if os.system("{0} < {1} > {2}".format(shlex.quote(converter_program_location), shlex.quote(filepath), shlex.quote(output_file))) != 0:
			sys.exit()
		print("Creating file '{0}'".format(output_image))
		os.system("{0} {2} < {1}".format(shlex.quote(image_writer_program_location), shlex.quote(output_file), shlex.quote(output_image)))