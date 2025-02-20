import requests
import time
import random
from concurrent.futures import ThreadPoolExecutor, as_completed

# Function to make a request and log the response time.
def make_request(size, request_num):
    url = f"http://localhost:10000/spin.cgi?size={size}"
    start_time = time.perf_counter()
    response = requests.get(url)
    duration = time.perf_counter() - start_time
    return request_num, size, duration

def main():
    num_requests_each = 1000
    sizes = [512, 1024]
    # Create a list of requests with specified sizes.
    requests_list = [random.choice(sizes) for _ in range(num_requests_each * 2)]

    # Shuffle the list if needed.
    random.shuffle(requests_list)

    results = []
    with ThreadPoolExecutor(max_workers=100) as executor:
        # Submit tasks concurrently.
        future_to_req = {executor.submit(make_request, size, i+1): (i+1, size)
                         for i, size in enumerate(requests_list)}
        for future in as_completed(future_to_req):
            req_num, size = future_to_req[future]
            try:
                result = future.result()
                results.append(result)
                # print(f"Request {result[0]}, size {result[1]}: {result[2]:.4f} seconds")
            except Exception as exc:
                print(f"Request {req_num} size {size} generated an exception: {exc}")

    # Write results to a CSV file.
    with open("response_times.csv", "w") as f:
        f.write("request_num,size,duration\n")
        for req_num, size, duration in results:
            f.write(f"{req_num},{size},{duration}\n")
    
    # Compute summary statistics.
    sizes_data = {512: [], 1024: []}
    for _, size, duration in results:
        sizes_data[size].append(duration)
    
    for size in sizes:
        if sizes_data[size]:
            avg_time = sum(sizes_data[size]) / len(sizes_data[size])
            print(f"Average response time for size {size}: {avg_time:.4f} seconds")

if __name__ == "__main__":
    main()
