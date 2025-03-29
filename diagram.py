import matplotlib.pyplot as plt

# Dữ liệu từ kết quả của bạn
sizes = [100, 200, 300, 400, 500]
threads = [0, 2, 4]

# Thời gian thực thi từ kết quả thực tế
execution_times = {
    100: [0.00791355, 0.011299, 0.00961091],
    200: [0.0780922, 0.0715596, 0.073077],
    300: [0.223099, 0.228929, 0.224268],
    400: [0.474972, 0.490444, 0.488562],
    500: [0.935504, 0.933048, 0.928164]
}

# Vẽ biểu đồ
plt.figure(figsize=(10, 6))  # Kích thước biểu đồ

for size in sizes:
    plt.plot(threads, execution_times[size], marker='o', label=f'{size}x{size}')

# Thiết lập tiêu đề và nhãn
plt.title('Execution Time vs Number of Threads for Different Matrix Sizes', fontsize=14)
plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Execution Time (seconds)', fontsize=12)
plt.legend(title='Matrix Size')
plt.grid(True)  # Thêm lưới để dễ đọc

# Hiển thị biểu đồ
plt.show()
