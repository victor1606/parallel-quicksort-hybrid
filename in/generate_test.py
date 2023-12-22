import random

def generate_and_write_input_file(file_path, array_size):
    try:
        with open(file_path, 'w') as file:
            file.write(str(array_size) + '\n')
            for _ in range(array_size):
                random_num = random.randint(0, 10**7)
                file.write(str(random_num) + ' ')
    except Exception as e:
        print(f"An error occurred: {str(e)}")

file_path = 'input.txt'
array_size = 6000000
generate_and_write_input_file(file_path, array_size)
