import datetime

def print_current_time():
    now = datetime.datetime.now()
    print(f"The current time is: {now.strftime('%Y-%m-%d %H:%M:%S')}")

if __name__ == "__main__":
    print_current_time()