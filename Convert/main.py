import polars as pl

print("开始转换文件！")
df = pl.scan_csv("../Data/app.log", 
                 separator="|", 
                 has_header=False, 
                 new_columns=["timestamp", "level", "user_id", "action", "latency_ms", "error_code"]
).with_columns(
    pl.all().str.strip_chars()
)

'''
real_df = df.collect()
print(real_df.shape)
print(real_df.schema)
print(real_df.head())
print(real_df.tail())
'''
df.sink_parquet("../Data/logs_output.parquet", compression="zstd")
print("转换完成文件: logs_output.parquet !")