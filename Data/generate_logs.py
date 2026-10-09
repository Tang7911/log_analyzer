from datetime import datetime, timedelta
import random

#获取当前时间
current_time = datetime.now()
#动作组
action = ["login", "view_item", "add_cart", "pay"]
#错误码组
error_codes = [404, 500, 502, 403]

#生成日志条数
total_lines = 10000
#写入文件，自动关闭文件回收资源
with open("app.log", "w", encoding="utf-8") as f:
    for i in range(total_lines):
        #格式化时间
        formatted_time = current_time.strftime("%Y-%m-%d %H:%M:%S")
        #随机生成UID
        user_uid = random.randint(1, 999)
        #随机动作
        user_action = random.choice(action)
        #随机结果
        result_probability = random.random()
        #随机耗时
        random_time_consuming = random.random()
        if random_time_consuming < 0.05:
            time_consuming = random.randint(800, 1500)
        else:
            time_consuming = random.randint(20, 100)
        
        #拼接生成日志
        if result_probability < 0.5:
            log_lines = f"{formatted_time} | ERROR | user_{user_uid} | {user_action} | {time_consuming}ms | {random.choice(error_codes)}\n"
        elif result_probability < 0.6:
            log_lines = f"{formatted_time} | WARN | user_{user_uid} | {user_action} | {time_consuming}ms | 0\n"
        else:
            log_lines = f"{formatted_time} | INFO | user_{user_uid} | {user_action} | {time_consuming}ms | 0\n"

        #写入日志
        f.write(log_lines)
        #随机时间间隔，保证时间随机
        delta = timedelta(seconds=random.randint(1, 3))
        current_time=current_time+delta