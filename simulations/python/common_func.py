import numpy as np

def remove_duplicates(x, y):
    unique_indices = []  # 用于存储去重后保留的索引
    seen = set()
    
    # 检查 y 最后一个重复的数字是否为 0
    y_count = np.count_nonzero(y == 0)
    last_zero_repeated = y_count > 1

    for i, yi in enumerate(y):
        if yi not in seen or (yi == 0 and last_zero_repeated):
            seen.add(yi)
            unique_indices.append(i)

    # 根据保留的索引创建新的 x 和 y
    unique_indices = np.array(unique_indices)
    new_x = x[unique_indices]
    new_y = y[unique_indices]

    return new_x, new_y

# # 示例数据
# x = np.array([1, 2, 3, 4, 5, 6])
# y = np.array([10, 20, 10, 30, 20, 0])

# new_x, new_y = remove_duplicates_with_condition(x, y)

# print("去重后的 x:", new_x)
# print("去重后的 y:", new_y)
