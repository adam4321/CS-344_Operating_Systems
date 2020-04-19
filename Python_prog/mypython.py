""" 
Author:         Adam Wright
Email:          wrighada@oregonstate.edu
Description:    Python Project for OSU Operating Systems spring 2020 cs-344

Python Version: Python 3
"""

import sys
import random
import string


# Create three files
f1 = open( "./file_1.txt", "w+" )
f2 = open( "./file_2.txt", "w+" )
f3 = open( "./file_3.txt", "w+" )

# Seed the random function
random.seed()

# Declare the random string length
string_length = 10

# Generate 3 random strings of lowercase ascii
str_1 = "".join( random.choices( string.ascii_lowercase, k = string_length )) 
str_2 = "".join( random.choices( string.ascii_lowercase, k = string_length ))  
str_3 = "".join( random.choices( string.ascii_lowercase, k = string_length ))  

# Overwrite the files with these strings
f1.write( str_1 + "\n" )
f2.write( str_2 + "\n" )
f3.write( str_3 + "\n" )

# Close the three files
f1.close()
f2.close()
f3.close()

# Print the file's contents
sys.stdout.write( str_1 + "\n" )
sys.stdout.write( str_2 + "\n" )
sys.stdout.write( str_3 + "\n" )

# Generate 2 random nums between 1 - 42 and calculate their product
int_1 = random.randint( 1, 42 )
int_2 = random.randint( 1, 42 )
product = int_1 * int_2

sys.stdout.write( str(int_1) + "\n" )
sys.stdout.write( str(int_2) + "\n" )
sys.stdout.write( str(product) + "\n" )
