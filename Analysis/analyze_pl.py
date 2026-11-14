import polars as pl
import matplotlib.pyplot as plt

parquet_path = "../Data/logs_output.parquet"
lf = pl.scan_parquet(parquet_path)
level_counts_lf = lf.group_by("level").agg(
    pl.len().alias("level_count")
)
action_counts_lf = lf.group_by("action").agg(
    pl.len().alias("action_count")
)
latency_ms_lf = lf.select(
    pl.col("latency_ms").mean().alias("latency_ms_mean"),
    pl.col("latency_ms").median().alias("latency_ms_median"),
    pl.col("latency_ms").max().alias("latency_ms_max"),
    pl.col("latency_ms").min().alias("latency_ms_min"),
    pl.col("latency_ms").quantile(0.95).alias("latency_ms_quantile")
)
'''
print(level_counts_lf.collect())
print(action_counts_lf.collect())
print(latency_ms_lf.collect())
'''
action_ms = lf.group_by("action").agg(
    pl.col("latency_ms").mean().alias("action_ms_maen"),
    pl.col("latency_ms").max().alias("action_ms_max")
)
error_ratio = lf.select(
    ((pl.col("level") == "ERROR").sum() / pl.len()).alias("error_ratio")
)
slow_ratio = lf.select(
    ((pl.col("latency_ms") > 800).sum() / pl.len()).alias("slow_ratio")
)
result = pl.collect_all([level_counts_lf, action_counts_lf, 
                         latency_ms_lf, action_ms, error_ratio, slow_ratio])

(
    level_df, action_df, latency_ms_df, 
    action_ms_df, error_ratio_df, slow_ratio_df
) = result
print(level_df)
print(action_df)
print(latency_ms_df)
print(action_ms_df)
print(error_ratio_df)
print(slow_ratio_df)