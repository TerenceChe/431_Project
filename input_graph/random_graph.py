import random
import sys

def random_to_write() -> str:
    number_probability = 0.3
    if random.random() < number_probability:
        return str(random.randint(0, 1000))
    else:
        return 'i'

def generate_random_graph(N, filename):
    with open(filename, 'w') as f:
        for row in range(N):
            for col in range(N):
                if row == col:
                    f.write('0')
                else:
                    f.write(random_to_write())
                f.write(' ')
            f.write('\n')

## simple command line to generate input files to create matrix of desired size
# N - is the number of nodes
# output file name
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py N input_absolute_filepath")
    else:
        try:
            N = int(sys.argv[1])
            filename = sys.argv[2]
            generate_random_graph(N, filename)
            print(f"Random graph with {N} nodes generated successfully in {filename}.")
        except ValueError:
            print("Error: N must be an integer.")