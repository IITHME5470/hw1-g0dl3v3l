import matplotlib.pyplot as plt

# Function to read data from the file and parse it
def read_timing_data(file_path):
    matrix_sizes = []
    avg_times = []

    with open(file_path, 'r') as file:
        for line in file:
            parts = line.strip().split()
            if len(parts) == 4:  # Ensuring correct format
                matrix_id = int(parts[1])  # Extracting MatrixID
                avg_time = float(parts[3])  # Extracting AvgTime
                
                matrix_sizes.append(matrix_id)
                avg_times.append(avg_time)

    return matrix_sizes, avg_times

# File path to the log file
file_path = 'timing_results.log'  # Update with your actual file path

# Read the timing data
matrix_sizes, avg_times = read_timing_data(file_path)

# Plotting the data
plt.figure(figsize=(8, 6))
plt.plot(matrix_sizes, avg_times, marker='o', linestyle='-', color='b', label='Avg Time')

plt.xlabel('Matrix Size (n)')
plt.ylabel('Average Execution Time (seconds)')
plt.title('Matrix Size vs. Average Execution Time')
plt.grid(True)
plt.legend()
plt.show()
