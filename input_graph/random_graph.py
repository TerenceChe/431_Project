import random

# How many nodes.
N = 100

# 70% chance of writing i, 30% of writing a valid integer.
# Returns a random string to write, either 'i' or some number like '123'.
def random_to_write() -> str:
    # INT_MAX = 0x7fffffff # NOTE do we want our code to be safe and handle large values?
    number_probability = 0.3
    if random.random() < number_probability:
        # return str(random.randint(0, INT_MAX - 1)) # Causes negative overflow.
        return str(random.randint(0, 1000))
    else:
        return 'i'
    
with open('random_graph.txt', 'w') as f:
    for row in range(N):
        for col in range(N):
            if row == col:
                f.write('0')
            else:
                f.write(random_to_write())
            f.write(' ')
        f.write('\n')
