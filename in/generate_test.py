import random

def generate_and_write_input_file(file_path, array_size):
    try:
        with open(file_path, 'w') as file:
            file.write(str(array_size) + '\n')  # Write the array size as the first line
            for _ in range(array_size):
                # Generate a random long integer (you can adjust the range as needed)
                random_num = random.randint(0, 10**7)
                file.write(str(random_num) + ' ')
    except Exception as e:
        print(f"An error occurred: {str(e)}")

# Example usage:
file_path = 'input.txt'  # Replace with the desired output file path
array_size = 5999999  # Replace with the desired array size
generate_and_write_input_file(file_path, array_size)
